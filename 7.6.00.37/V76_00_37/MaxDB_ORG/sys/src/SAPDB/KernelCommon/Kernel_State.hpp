/*! @ingroup   KernelAdministration
    @file      Kernel_State.hpp
    @author    UweH
    @brief     This defines class Kernel_State
*/
/*!
\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end

\endif
*/
#ifndef Kernel_State_H
#define Kernel_State_H

#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
/*!
    @class Kernel_State
    @brief The kernels state is handled here.
 */
class Kernel_State
{
public:
    /// The kernel can operate in two main modes: online and admin.
    enum KernelState
    {
        kernelStateAdmin,
        kernelStateOnline
	};
    /// Kinds of critical states which must be guarded from parallel execution
    enum CriticalStates
    {
        criticalStateNone,
        criticalStateRedo,
        criticalStateBackup,
        criticalStateConfiguration,
        criticalStateCheckData,
        criticalStateAddDataVolume,
        criticalStateDropDataVolume
	};
    /// Kinds of standby role
    enum StandbyRoles
    {
        standbyRoleNone,
        standbyRoleMaster,
        standbyRoleStandby
    };
public:
    /// initializer
    Kernel_State(RTESync_NamedSpinlock &spinlock)
    : m_KernelState(kernelStateAdmin),
      m_StandbyRole(standbyRoleNone),
      m_IsRedoState(false),
      m_IsBackupState(false),
      m_IsConfigurationState(false),
      m_IsCheckDataState(false),
      m_IsAddDataVolumeState(false),
      m_IsDropDataVolumeState(false),
      m_SpinLock(spinlock)
    {}
    /// This enters a given critical state.
    /// If this returns false the redo phase cannot begin.
    bool EnterCriticalState(CriticalStates state)
    {
        switch (state)
        {
            case criticalStateRedo:
                return BeginCriticalState(m_IsRedoState);
            case criticalStateBackup:
                return BeginCriticalState(m_IsBackupState);
            case criticalStateConfiguration:
                return BeginCriticalState(m_IsConfigurationState);
            case criticalStateCheckData:
                return BeginCriticalState(m_IsCheckDataState);
            case criticalStateAddDataVolume:
                return BeginCriticalState(m_IsAddDataVolumeState);
            case criticalStateDropDataVolume:
                return BeginCriticalState(m_IsDropDataVolumeState);
            default:
                return false;
        }
    }
    /// This leaves a given critical state.
    void LeaveCriticalState(CriticalStates state)
    {
        switch (state)
        {
            case criticalStateRedo:
                m_IsRedoState = false;
                break;
            case criticalStateBackup:
                m_IsBackupState = false;
                break;
            case criticalStateConfiguration:
                m_IsConfigurationState = false;
                break;
            case criticalStateCheckData:
                m_IsCheckDataState = false;
                break;
            case criticalStateAddDataVolume:
                m_IsAddDataVolumeState = false;
                break;
            case criticalStateDropDataVolume:
                m_IsDropDataVolumeState = false;
                break;
            default:
                break;
        }
    }
    /// This returns true if the given critical state is entered.
    bool IsCriticalState(CriticalStates state)
    {
        switch (state)
        {
            case criticalStateRedo:
                return m_IsRedoState;
            case criticalStateBackup:
                return m_IsBackupState;
            case criticalStateConfiguration:
                return m_IsConfigurationState;
            case criticalStateCheckData:
                return m_IsCheckDataState;
            case criticalStateAddDataVolume:
                return m_IsAddDataVolumeState;
            case criticalStateDropDataVolume:
                return m_IsDropDataVolumeState;
            default:
                return false;
        }
    }
    /// Set kernel state to online (user can connect)
    /// Implicitely the redo phase ends.
    void SetOnline()
    {
        m_IsRedoState = false;
        m_KernelState = kernelStateOnline;
    }
    /// Set hot standby role
    void SetHotStandbyRole(StandbyRoles newRole)
    {
        m_StandbyRole = newRole;
    }
    /// Get hot standby role
    StandbyRoles GetHotStandbyRole() const
    {
        return m_StandbyRole;
    }
    /// Returns the kernel state.
    KernelState GetKernelState()
    {
        return m_KernelState;
    }
    /// This is a shortcut for convenience
    bool IsRedoLogProcessing()
    {
        return m_IsRedoState;
    }
    /// This is a shortcut for convenience
    bool IsAdmin()
    {
        return m_KernelState == kernelStateAdmin;
    }
    /// This is a shortcut for convenience
    bool IsOnline()
    {
        return m_KernelState == kernelStateOnline;
    }
private:
    /// returns true if the state could be reached else false
    bool BeginCriticalState (bool &state)
    {
        bool stateCanBeReached;
        m_SpinLock.Lock();
        stateCanBeReached = ! state;
        if ( stateCanBeReached )
            state = true;
        m_SpinLock.Unlock();
        return stateCanBeReached;
    }
private:
    /// This is the flag which describes if the kernel operates in admin or online mode.
    KernelState m_KernelState;
    /// a kernel can play a role in a hotstandby configuration
    StandbyRoles m_StandbyRole;
    /// This is only true during redo of log.
    bool m_IsRedoState;
    /// This is true if any backup command is already running
    bool m_IsBackupState;
    /// This is true if any configuration command is already running
    bool m_IsConfigurationState;
    /// This is true if check data (kernelStateOnline) or check data with update (kernelStateAdmin) is running
    bool m_IsCheckDataState;
    /// This is true if add volume is running
    bool m_IsAddDataVolumeState;
    /// This is true if add volume is running
    bool m_IsDropDataVolumeState;
    /// needed for guarding
    RTESync_NamedSpinlock &m_SpinLock;
};
/*!
    @class Kernel_StateScope
    @brief This is a convenient function. The destructor leaves the critical state if possible.
 */
class Kernel_StateScope
{
public:
    /// constructor initializes all members
    Kernel_StateScope(Kernel_State::CriticalStates  criticalState,
                      Kernel_State                 &state)
    : m_CriticalState(criticalState),
      m_State        (state),
      m_Entered      (false)
    {}
    /// This enters the constructed critical state.
    /// If false is returned, the entering failed.
    bool EnterCriticalState()
    {
        return m_Entered = m_State.EnterCriticalState(m_CriticalState);
    }
    /// Explicitely leaves the critical state.
    void LeaveCriticalState()
    {
        m_State.LeaveCriticalState(m_CriticalState);
        m_Entered = false;
    }
    /// Implicitly leaves the critical state.
    ~Kernel_StateScope()
    {
        if ( m_Entered )
            LeaveCriticalState();
    }
private:
    /// The given critical state.
    Kernel_State::CriticalStates  m_CriticalState;
    /// The global state of the kernel
    Kernel_State                 &m_State;
    /// The flag if we entered the critical state or not.
    bool                          m_Entered;
};
#endif  /* Kernel_State_H */
