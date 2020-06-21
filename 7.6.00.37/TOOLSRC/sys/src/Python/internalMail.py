
import sys
import smtplib
import traceback
import cStringIO

_mailhost = 'mail.sap-ag.de'

def sendMail (fromaddr, toaddr, subject, msg):
    server = smtplib.SMTP (_mailhost)
    server.sendmail(fromaddr, toaddr, 'Subject:' + subject + '\n\n' + msg)
    server.quit()

def sendFailure (fromaddr, toaddr, subject, excInfo = None):
    msg = cStringIO.StringIO ()
    if excInfo is None:
        type, value, stack = sys.exc_info ()
    else:
        type, value, stack = excInfo
    traceback.print_exception(type, value, stack, None, msg)
    sendMail (fromaddr, toaddr, 'ERR: ' + subject, msg.getvalue ())

if __name__ == "__main__":
    testAddr = 'daniel.dittmar@sap.com'
    if None:
        sendMail (testAddr, testAddr, 'Test sendMail', """line 1
line 2
        """)
    if 1:
        try:
            import re
            re.compile ('dfdf (dfdfdf')
        except:
            sendFailure (testAddr, testAddr, 're test')
            raise

