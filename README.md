# Online-Railway-Ticket-Booking-System

This project is done, entirely using system calls in C language and files to store the booking and user details.


1. There are three files: client.c, server.c and util.h. 

2. client.c serves as the client side, server.c serves as the server side ,for the ticket booking system and util.h contains all the functions required for the application to run.
	client.c - User Interface code.
	server.c - Server accepts data from client,  performs validation check with help of databases, and utilities. Responds to requests by  clients.
	
	
# DATABASE FILES:

1. account.txt: This files,stores details regarding users. Userid ,password along with the type of user.
2. train.txt: This files, stores train details.
3. booking.txt: This file stores all ticket bookings made for trains by users.
4. vacancies.txt: This file stores details , for cancelled tickets, whihch helps in giving away seats to new users from the pool of cancelled tickets.


# TYPES OF USERS

1. NORMAL USER:  You will be able to book tickets, view bookings, perform any updates . Restrict him to login from only one terminal. 
2. AGENT:  Just like normal user, except that, allow him to login simultaneously ,and carry out operations accordingly.
3. ADMIN:  Being System administrator, he has right to add, delete , modify trains and also users. 


CONCURRENT SERVER:

Server is designed to handle multiple requests from clients, for each client request,a thread is created, for further interactions with client.


LOCK MECHANISMS:

When server interacts with database,  to server client requests, appropriate locking mechanisms is employed , such as write lock, read lock.


# STEPS TO RUN:

1. Open 2 terminals one for running server.c and other for running client.c

2. First compile and run server.c and then compile and run client.c


3. To compile Server:	cc -pthread server.c -o s
   To run Server:	./s
   	
4. To compile client:	cc client.c
   To run client:	./a.out


5. After compiling and running the server side, you will be prompted to sign up yourself.
6. After signing up, you can login. You will be prompted with further choices, based on type of user.
	
	
7. If normal user or agent , you will be prompted to choose one among the five options: 
	
		1. Book Ticket
		2. View previous Bookings
		3. Update Booking
		4. Cancel Booking
		5. Logout
		
8. If normal admin , you will be prompted to choose one among the five options: 
		1. Add train details
		2. Delete train details
		3. Update train details
		4. Search trains
		5. Delete user
		6. Add user
		7. Search User
		8. Train's Booking list
		9. Logout
                
		
9. Once you choose to logout, you can exit the session.

