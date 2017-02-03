The client app sends a message about the processes and memory status of the Windows to the server

*ConsoleAplication1/ is the Client app VS project dir.
*The file ConsoleApplication1/app.cfg defines the settings (server address, user email, user pass) 
for the client. take a look at that for see the structure the test must follows
*RestService/ is the server app dir.
*This project is an application of C++ API REST (codenamed 'Casablanca')
*the file RestService/users.txt is the server storage of clients authorized to send data to it.