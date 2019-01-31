#!/bin/bash -x

SCRIPTDIR=$(dirname $0)
STOREPASS=changeit
CA_ALIAS=owl_ca

# clean up
rm *.pem *.der
rm -rf private/* signedcerts/*
rm keystore.jks
keytool -delete -keystore cacerts.jks -alias s1as -storepass $STOREPASS

# create directories
mkdir private
mkdir signedcerts

# create CA cert
export OPENSSL_CONF=$SCRIPTDIR/caconfig.conf
openssl req -x509 -newkey rsa:2048 -out cacert.pem -outform PEM -days 1825

# convert and import the ca certificate 
openssl x509 -in cacert.pem -inform pem -out cacert.der -outform der
keytool -importcert -alias s1as -file cacert.der -keystore cacerts.jks -storepass $STOREPASS
keytool -importcert -trustcacerts -alias $CA_ALIAS -file cacert.der -keystore keystore.jks -storepass $STOREPASS

# create csr
keytool -genkey -alias s1as -keyalg RSA -keystore keystore.jks -storepass $STOREPASS
keytool -certreq -alias s1as -keyalg RSA  -file certreq.pem -keystore keystore.jks -storepass $STOREPASS

# sign the csr and import the result
openssl ca -in certreq.pem -out server_crt.pem
openssl x509 -in server_crt.pem -inform pem -out server_crt.der -outform der
keytool -importcert -trustcacerts -alias s1as -keystore keystore.jks -file server_crt.der -storepass $STOREPASS

