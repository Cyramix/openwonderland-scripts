SSL support for Open Wonderland
Version 0, December 10th, 2010

OVERVIEW
--------
These patches add SSL support to the Open Wonderland web server. In particular, after applying these patches, all authentication credentials will be sent to the web server using SSL.

The encryption in these patches only applies to the web server. Data to the Darkstar, Voice and Shared Application servers are still unencrypted. Note, however, that authentication credentials (username and password) are only sent to the web server. All other insecure services use a temporary token to get access, so after this patch is applied, your username and password will always be sent encrypted.


INSTRUCTIONS
------------
To install SSL support, you will need to apply patches to the Open Wonderland source code. Once the patches are installed, you will need to generate a secure certificate, and configure several services to use this certificate. 

In these instructions, we will cover how to create a working self-signed certificate. For real deployments, you will need to get an authentic signed certificate from a commercial certificate authority.


PRE-REQUISITES
--------------
These patches assume you have a working installation of Open Wonderland from source code, with a version similar to the version numbers listed below. Further, authentication must be set up as described in this tutorial:

http://wiki.java.net/bin/view/Javadesktop/ProjectWonderlandAuthentication05

In addition, you will need the openssl toolkit for generating keys:

http://www.openssl.org/


APPLYING PATCHES
----------------
The ssl-core.diff and ssl-modules.diff patches included in this bundle were generated against svn revision 4585 of the openwonderland workspace and revision 1617 of the openwonderland-modules workspace.

1. In the wonderland/ directory, apply the ssl-core.diff patch using NetBeans (Team -> Apply Diff Patch) or the command line (patch -p0 < ssl-core.diff).

2. In the wonderland-modules/ directory apply the ssl-modules.diff patch using the same mechanism.


CONFIGURE THE CERTIFICATE AUTHORITY
-----------------------------------
The ca/ directory of this bundle contains tools for creating appropriate keystores for use in the Open Wonderland components. 

Before creating the keystore, you will need to configure your certificate authority. Do this by copying the file "caconfig.in" to one named "caconfig.conf". Now edit the caconfig.conf file, and change the following values:

1. dir = /FULL/PATH/TO/ssl/ca
   Replace with the absolute path to the ca/ directory. For example:
   dir = /Users/jkaplan/Desktop/ssl/ca

2. subjectAltName = IP:AA.BB.CC.DD,DNS:REPLACEME.COM
   Replace with the actual IP address and/or dns name by which clients
   will connect to your server. Clients will only connect to servers
   that match the subjectAltName specified in this configuration. This
   must be kept up-to-date for every server you deploy to. Multiple
   values may be specified, separated by commas. For example:
   subjectAltName = IP:192.168.3.42,IP:10.0.1.1,DNS:wl.mydomain.com

3. default_keyfile = /FULL/PATH/TO/ssl/ca/private/cakey.pem
   Replace with the absolute path as above, for example:
   default_keyfile = /Users/jkaplan/Desktop/ssl/ca/private/cakey.pem

More information on configuration certificate generation in OpenSSL is available here:

http://www.openssl.org/docs/apps/openssl.html


GENERATE CERTIFICATES
---------------------
Now that the certificate authority is configured, run the ca.sh shell script to generate the necessary keys. You will be prompted to answer a number of questions during the process:

Enter PEM pass phrase: <-- enter any password you will remember
Verifying - Enter PEM pass phrase: <-- enter the same phrase again

Trust this certificate? [no]: <-- verify the CA info above and enter "yes"
Trust this certificate? [no]: <-- enter "yes" again to trust the CA

What is your first and last name? 
  [Unknown]: <-- for example: Open Wonderland
What is the name of your organizational unit? 
  [Unknown]: <-- for example: Open Wonderland Foundation
What is the name of your organization?
  [Unknown]: <-- for example: Open Wonderland
What is the name of your City or Locality?
  [Unknown]: <-- for example: San Francisco
What is the name of your State or Province?
  [Unknown]: <-- for example: CA
What is the two-letter country code for this unit?
  [Unknown]: <-- for example: US
Is CN=Open Wonderland, OU=Open Wonderland Foundation, ... correct?
  [no]: <--  verify certificate and enter "yes"

Enter key password for <s1as>
	(RETURN if same as keystore password): <-- press RETURN

Enter pass phrase for /Users/jkaplan/Desktop/ssl/ca/private/cakey.pem:
 <-- enter passphrase you entered in the first question

Sign the certificate? [y/n]: <-- enter "y"
1 out of 1 certificate requests certified, commit? [y/n] <-- enter "y"


After answering all these questions, you should have two key files
in the ca/ directory: keystore.jks and cacerts.jks.  The first contains
the certificate the webserver will present when making SSL connections.
The second is a modified version of the standard Java trusted 
certificate authorities list, with the our newly created certificate
authority included.


CONFIGURE OPEN WONDERLAND
-------------------------
The final step is to configure the Open Wonderland server to use these
new keys. Make the following modifications to wonderland/my.run.properties:

javax.net.ssl.trustStore=/FULL/PATH/TO/ssl/ca/cacerts.jks
javax.net.ssl.keyStore=/FULL/PATH/TO/ssl/ca/keystore.jks

Replace these paths with the absolute path of the cacerts and
keystore files.

Now start the Open Wonderland server using "ant run-server". Once the
server starts up, go to the front page and select "Server Admin". Note
that the login dialog is now secure. You may get an error about the fact
that the page is using a self-signed certificate, but in most browsers
you should be able to bypass this (in Chrome, you may need to open the
red login frame in a new window to continue).

In the web administration UI, you will notice that the Darkstar server has
failed to start up because it does not recognize our new certificate
authority. Add the following to the properties of all services
(Voice Bridge, Darkstar Server and Shared Application Server):

javax.net.ssl.trustStore=/FULL/PATH/TO/ssl/ca/cacerts.jks

Stop and restart all services, and they should all start up properly.

Congratulations, your Open Wonderland web server is now serving
authentication data over SSL!


KNOWN ISSUES
------------

1. Due to the use of self-signed certificates, many warnings will be shown to the user. To avoid these warnings, use a certificate signed by a 
trusted commercial certificate authority.

2. Problems downloading jars over SSL. If you log in to the web administration page, and then click "home", you will be directed to
a secure version of the Open Wonderland launch page. If you proceed to launch the client, you may get an error while downloading the jar files.  To workaround this problem, load the non-secure version of the launch page by manually entering the server address and insecure port number into your web browser.
