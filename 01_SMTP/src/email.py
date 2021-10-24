from netcat import Netcat

#creating object of Netcat Class and initiating connection with the given hostname and port number
nc = Netcat()
nc.connect('mail-relay.iu.edu', 25)

#initiating a client-server SMTP session (aka conversation) - [include client domain or IP]
nc.write('\r HELO nkamble \n')
#print(nc.read())

#initiating mail transfer - [passing sender email_id as argument]
fro = input('Enter Source Email ID: ')
nc.write('\r MAIL FROM:<' + fro + '> \n')
#print(nc.read())

#command specifies the recipient email_id
too = input('Enter Destination Email ID: ')
nc.write('\r RCPT TO:<' + too + '> \n')
#print(nc.read())

#this command is used to ask the server for permission to transfer the mail data
nc.write('\r DATA \n')

#to mention the subject
subj = input('Enter Subject: ')
nc.write('subject: ' + subj)

#Email Body [includes Body text in raw format]
fname = input('Enter the filename that includes Email Content: ')
fila = open(fname)

#attaching Email Body inside Mail-Data
nc.write(fila.read())
#print(nc.read())

fila.close()
nc.close()
print('Email Delivered')
