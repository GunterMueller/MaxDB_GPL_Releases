Contents

1.    Introduction
2.    WebDAV landscape
3.    How to deploy the MaxDB WebDAV Servlet
3.1.    General overview
3.1.1.    Prerequisites
3.1.2.    Which file contains what
3.2.    Tomcat
3.3.    SAP NetWeaver 6.40
3.4.    Others




1. Introduction

The MaxDB WebDAV Servlet is an implementation of the WebDAV protocol as defined on RFC2518. With the help of the MaxDB WebDAV Servlet, a MaxDB database instance can be used as storage for documents, which users can access and manipulate using the WebDAV protocol. 

For details of the WebDAV protocol, visit www.w3.org

Only deployment of the WebDAV Servlet on Tomcat and SAP NetWeaver is indicated below. For detailed information concerning deployment on these servers as well as on your J2EE server consult the respective manufacturer’s documentation. 


2. WebDAV landscape

A MaxDB WebDAV environment consists of a MaxDB database instance of a suitable size, the MaxDB WebDAV Servlet, as well as a J2EE server or servlet container like Tomcat, SAP NetWeaver, JBoss or others. The MaxDB database instance serves as storage for the documents. The MaxDB WebDAV Servlet provides the interface to these documents and the J2EE server provides the runtime environment, in which the MaxDB WebDAV Servlet runs.


3. How to deploy MaxDB WebDAV Servlet

3.1. General Overview

The deployment of servlets differs substantially in specific J2EE servers. The most common difference is the utilization of different configuration files. This, as a matter of fact, impedes the simple “Ready-to-go” installation of the MaxDB WebDAV Servlet. In order to assist the J2EE administrator, the MaxDB WebDAV Servlet is shipped with Tomcat, SAP NetWeaver and JBoss with configuration files. These files must be partially adjusted to the specific circumstances of the installation. Thus in each case the data source is to be configured, so that it works with an existing MaxDB database instance. Furthermore, in the case of SAP NetWeaver, the deployment descriptor must contain the administrator’s registration data.


3.1.1. Prerequisites

The prerequisites for using the MaxDB WebDAV Servlet are an existing MaxDB installation, an existing MaxDB instance, a J2EE server, the MaxDB JDBC driver as well as the MaxDB WebDAV Servlet itself. The MaxDB JDBC driver must be made known to the J2EE server or servlet container. To determine how to do this, refer to your server’s documentation. 


3.1.2. Which file contains what

The MaxDB WebDAV Servlet packet contains several files which are required for the deployment of the WebDAV Servlet. Not all of these files are required for the deployment on each J2EE server. Listed below are the individual files together with a description of their usage:


webdavServletMaxDB-1.0.war	

The .war file is required for deployment on, for example, Tomcat or JBoss. It contains the servlet as well as a number of configuration files. These configuration files are:

- web.xml: This file is the so-called Deployment Descriptor. It specifies how the servlet should be deployed by the server. It also contains the configuration parameters, which the servlet reads. In addition it defines the data source. In the case of the MaxDB WebDAV Servlet, it relates to the data source with the name “jdbc/WebDAVDB”

- context.xml: This file defines the context path, under which the servlet is accessible on the server, as well as the connection parameters for the data source defined in web.xml, which the servlet uses for its communication with the database. 

- jboss-web.xml: This file is only required for deployment onto a JBoss J2EE server. It contains JBoss specific configuration parameters for the deployment. In the case of the MaxDB WebDAV Servlet, it merely consists of a resource reference with data source name mapping on a JNDI name. For a definition of the data source for JBoss see WebDAVDB-ds.xml


webdavcontext.xml

This file is an external version of context.xml. After installation of the MaxDB WebDAV Servlet, it contains the same information as the file context.xml, which is contained in the file webdavServletMaxDB-1.0.war. Some servers such as Tomcat allow a declaration of an alternative context file upon deployment. This option can be used to adjust the configuration parameters to the actual landscape before the deployment.


WebDAVDB-ds.xml

This file is only required for deployment on JBoss. It contains the access parameters for the WebDAVDB data source, which has been defined in jboss-web.xml under that name. If the name of the data source in jboss-web.xml changes, a new corresponding <datasourcename>-ds.xml must be generated.


webdavServletMaxDB-1.0.ear	

The .ear file is required for deployment on SAP NetWeaver. It consists of the servlet and some configuration files. These configuration files are:

- application.xml: This file determines the servlet’s „Display Name“. Furthermore it determines the .war file, which contains the servlet as well as the context path, by which the servlet can be called after deployment. 

- data-sources.xml: On the one hand this file allocates the data source to an application, on the other hand it contains the configuration and connection parameters of the session pool, which the servlet will later use. The parameters can subsequently be changed with the help of the SAP NetWeaver tools. For further information consult the SAP NetWeaver documentation. 


webdavServletMaxDB-1.0.xml

This file is the SAP NetWeaver deployment descriptor. It determines the .ear file to be deployed or specifies the source directory for the .ear. For further information consult the SAP NetWeaver documentation.


data-sources.xml

On the one hand this file allocates the data source to an application, on the other hand it contains the configuration and connection parameters of the session pool, which the servlet will later use. With the help of SAP NetWeaver’s telnet based configuration, a data source can be deployed on the server. This file is required for this purpose. For further information consult the SAP NetWeaver documentation. 


3.2. Tomcat

As a basic principle, deployment on Tomcat can take the following two forms. 

a) You copy the file webdavServletMaxDB-1.0.war into the directory webapps and wait until Tomcat automatically deploys it. 

b) You use the Tomcat Manager for the deployment. In this case you can either deploy the .war file as it is, or you indicate a context path and an alternative context file of your choice alongside the .war file. A prepared alternative context file is already contained in the delivery of the MaxDB WebDAV servlet. You only need to enter the connection parameters for the storage database. For further information on deployment with Tomcat consult the Tomcat documentation. 


3.3. SAP NetWeaver 6.40

Deployment on SAP NetWeaver can take the following three forms:

a) Using the specific SAP NetWeaver deployment tool
b) Using the specific SAP NetWeaver command line tool DeployManager
c) Using the SAP NetWeaver telnet administration

For all three methods, the necessary configuration files and archives are contained in the shipment of the MaxDB WebDAV Servlet. 

You customize these files to suit your installation, for example you add connection parameters of the MaxDB instance, which forms the document store. For further information and the exact procedure consult the SAP NetWeaver documentation. 


3.4. Others

The MaxDB WebDAV Servlet can also be used with other J2EE servers. For information relating to the respective configuration files and deployment steps please consult the documentation of the respective manufacturer. 

