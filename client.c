/*
 *	CLIENT SIDE PROGRAM FOR ONLINE RAILWAY TICKET BOOKING SYSTEM
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include"util.h"
#define PORT 4466

void ischoicelogin(int);
void ischoiceregister(int);

void adminmenu(int,struct client);
void addtraindetails(int );
void deletetraindetails(int);
void updatetraindetails(int);
void searchtrain(int );
void deleteuserdetails(int ,struct client );
int adduserdetails(int);
void searchuserdetails(int );
void bookingontrainid(int);


void usermenu(int,struct client);
void booktickets(int ,struct client);
void prevbookings(int ,struct client );
void cancelbooking(int ,struct client );
void increase_or_decrease_seats(int,struct client );

//	ADMIN FUNCTIONS

void addtraindetails(int clisock)
{
	struct train tnew;
	printf("Enter train details to be inserted\n");
	tnew.train_id=-1;
	printf("Train name : ");
	scanf("%s",tnew.trainname);
	printf("Total Seats : ");
	scanf("%d",&tnew.seats);
	
	tnew.lastseatalloted=0;
	
	write(clisock,&tnew,sizeof(tnew));
			
	read(clisock,&tnew,sizeof(tnew));
	printf("\n\nTrain added succesfully\n");
	printf("ID\tNAME\t\tAVAILABLE_SEATS\n");
	printf("%d\t%s\t%d\n",tnew.train_id,tnew.trainname,tnew.seats);
	
}

void deletetraindetails(int clisock)
{
	int trainid;

	printf("\nEnter train-id, which is to be deleted\n");
	scanf("%d",&trainid);
	write(clisock,&trainid,sizeof(trainid));
	int delstat;
	read(clisock,&delstat,sizeof(delstat));
	if(delstat == -1)
		printf("\n\nCould not delete succesfully, either train with given id is present or train is already deleted\n");
	else
		printf("\n\nDeleted successfully\n");	
}

void updatetraindetails(int clisock)
{
	struct bookticket b;
	//int tr_num,seats;
	struct train tr;
		
	int size;
	read(clisock,&size,sizeof(size));	
	//printf("size= %d\n",size);
	while(size > 0)
	{
		read(clisock,&size,sizeof(size));	
		//printf("%d\n",size);
		read(clisock,&tr,sizeof(tr));		
		if(strcmp(tr.trainname,"removed") !=0)
			printf("%d\t%s\t%d\t%d\n",tr.train_id,tr.trainname,tr.seats,tr.lastseatalloted);
	}
	
	
	int trainid,seats,increased;
	
	//update train details
	printf("\nEnter train id, whose seats is supposed to be increased or decreased\n");

	scanf("%d",&trainid);
	printf("Enter number of seats\n");
	scanf("%d",&seats);
	printf("Enter 1. Increase\n2. Decrease\n");
	scanf("%d",&increased);
	write(clisock,&trainid,sizeof(trainid));
	write(clisock,&seats,sizeof(seats));
	write(clisock,&increased,sizeof(increased));
	int stat;
	read(clisock,&stat,sizeof(stat));
	if(stat == 1)
		printf("\n\nUpdated successfully\n");
	else
		printf("\n\nCould not update.\n");
}

void searchtrain(int clisock)
{

	printf("\nEnter train ID, you want to search for\n");
	int trainid;
  	scanf("%d",&trainid);

	write(clisock,&trainid,sizeof(trainid));
	
	struct train stat;
	read(clisock,&stat,sizeof(stat));
	if(stat.train_id == -1)
		printf("\n\nRequested train is not available. It could have been cancelled.\n");
	else
	{
		printf("ID\tName\tSeats\tLast_Seat_Alloted\n");
		printf("%d\t%s\t%d\t%d\n",stat.train_id,stat.trainname,stat.seats,stat.lastseatalloted);
	}
}

void deleteuserdetails(int clisock,struct client c1)
{
	struct flock l;
	l.l_type=F_WRLCK;
	l.l_len=0;
	l.l_start=0;
	l.l_whence=SEEK_SET;
	l.l_pid=getpid();
	int fd=open("account.txt",O_RDONLY);
	fcntl(fd,F_SETLKW,&l);
	struct client c;
	printf("User_ID\tPassword\tType\n");
	
	while(read(fd,&c,sizeof(c)) >0)
	{
		//printf("inside\n");

		printf("%d\t\t%d\t\t",c.userid,c.password);		
		if(c.type == 1)
			printf("USER\n");
		else if(c.type == 2)
			printf("AGENT\n");
		else if(c.type == 3)
			printf("ADMIN\n");
	}

	l.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&l);
	close(fd);

	printf("\nProvide USERID for users you want to delete\n");
	int uid;
	scanf("%d",&uid);
	write(clisock,&uid,sizeof(uid));
	write(clisock,&c1,sizeof(c1));
	int stat;
	read(clisock,&stat,sizeof(stat));
	if(stat == 1)
		printf("\n\nUser deleted successfully\n");
	else
		printf("\n\nMake sure you have entered correct USERID.\n");

}

int adduserdetails(int clientSocket)
{
	int type,userid,password;		
	printf("Enter the type of account :\n");
	printf("1. User\t2. Agent\t3. Admin\n");
	printf("Your Choice: ");
	scanf("%d", &type);
			

	if(type > 4 || type < 1)
	{

		printf("Invalid Choice!\n");
		//exit(0);
		return -1;
	}

	//printf("CS=%d",clientSocket);
	write(clientSocket, &type, sizeof(type));
	//perror("");

	printf("Enter userid : ");
	scanf("%d",&userid);

	
	write(clientSocket, &userid, sizeof(userid));
	//perror("");

	printf("Enter password : ");

	scanf("%d",&password);

	write(clientSocket, &password, sizeof(password));
	//perror("");
	int regstat;

	read(clientSocket,&regstat,sizeof(regstat));

	if(regstat == 1)
	{
		printf("\n\nRegistered successfully, you can now login\n");
	}
	else
	{
		printf("\n\nCould not register,please try to change userid or password, and reregister, yourself.\n");

	}
}

void searchuserdetails(int socket)
{
	int uid;
	
	printf("\nEnter user's id, for which you are requesting the details\n");
	scanf("%d",&uid);
	write(socket,&uid,sizeof(uid));
	int size,flag;
	read(socket,&flag,sizeof(flag));
	if(flag == 0)
		printf("\nUser details incorrect.\n");
	else
	{
		struct client c;
		read(socket,&c,sizeof(c));
		printf("\nCustomer found.\nHere are the details\n\n");
                printf("UserId\tPassword\tType\n");
                printf("%d\t%d\t\t",c.userid,c.password);
                if(c.type == 1)
                        printf("USER\n");
                else if(c.type == 2)
                        printf("AGENT\n");
                else if(c.type == 3)
                        printf("ADMIN\n");
		printf("\n");
	}

}
void bookingontrainid(int sock)
{
	struct bookingDetails b;
	int tr_num,seats;
	struct train tr;
		
	int size;
	read(sock,&size,sizeof(size));	
	//printf("size= %d\n",size);
	printf("TrainId\tTrain_name\tAvailable_seats\tLastAllotedSeat\n");
	while(size > 0)
	{
		//printf("%d\n",size);
		read(sock,&tr,sizeof(tr));		
		read(sock,&size,sizeof(size));	
		
		if(strcmp(tr.trainname,"removed") !=0)
			printf("%d\t%s\t\t%d\t\t%d\n",tr.train_id,tr.trainname,tr.seats,tr.lastseatalloted);
	}

	printf("\nEnter train id, to view it's booking list\n");
	int tid;
	scanf("%d",&tid);
	
	write(sock,&tid,sizeof(tid));
	read(sock,&size,sizeof(size));
	
	printf("BookingId\tUserID\tTrainID\tTrain_Name\tBooking_Status\tSeat_No.\n");
	while(size >0)
	{
		read(sock,&size,sizeof(size));
		read(sock,&b,sizeof(b));
		if(b.trainid == tid)
		{
			printf("%d\t\t%d\t%d\t\t%s\t",b.bookingid,b.userid,b.trainid,b.trainname);
			if(b.bookingstatus == 'C')
				printf("CNF");				
			else if(b.bookingstatus == 'X')
				printf("Cancel");
			printf("\t\t%d\n",b.seatnumberalloted);
		}
	}
}

void adminmenu(int clisock,struct client c)
{
	while(1)
	{
		printf("\n\n1. Add train details\n2. Delete train details\n3. Update train details\n4. Search trains\n5. Delete user\n6. Add user\n7. Search User\n8. Train's Booking list.\n9. Logout\n\n");
		printf("Your choice : ");
		int option;
		scanf("%d",&option);
		if(option<1 || option>9)
		{
			printf("\n\nPlease enter valid choice\n");
			//exit(0);
		}
		else
		{
			write(clisock,&option,sizeof(option));
			if(option == 1)
			{
				addtraindetails(clisock);
			}
			else if(option == 2)
			{
				deletetraindetails(clisock);
			}
			else if(option == 3)
			{
				updatetraindetails(clisock);

			}
			else if(option == 4)
			{
				searchtrain(clisock);
			}
			else if(option == 5)
			{
				deleteuserdetails(clisock,c);
			}
			else if(option == 6)
			{
				adduserdetails(clisock);
			
			}
			else if(option == 7)
			{	
				searchuserdetails(clisock);	
			}
			else if(option == 8)
			{
				bookingontrainid(clisock);
			}
			else if(option == 9)
			{
				close(clisock);
				exit(0);
			}
		}
	}

}


//	USER FUNCTIONS 


void prevbookings(int clisock,struct client c)
{
	//if(isUserValid(c) != -1)
	//{	

	printf("\n\nYour previous bookings are as follows\n");
	printf("\n\n");
	//printf("Clisock = %d\n",clisock);
	int size;
	
	//view prev bookings
	write(clisock,&c,sizeof(c));
	//perror("");
	
	read(clisock,&size,sizeof(size));
	//perror("");
	
	//printf("res of size read in cli= %d\n",size);
	
	struct bookingDetails b;
	printf("BookingID\tUserId\tTrain_ID\tTrain-name\tBooking_Status\tSeat_Num\n");
	
	while(size >0)
	{
		read(clisock,&size,sizeof(size));
		read(clisock,&b,sizeof(b));
		if(b.userid == c.userid)
		{
			printf("%d\t\t%d\t%d\t\t%s\t",b.bookingid,b.userid,b.trainid,b.trainname);
			if(b.bookingstatus == 'C')
				printf("CNF");				
			else if(b.bookingstatus == 'X')
				printf("Cancel");
		
			printf("\t\t%d\n",b.seatnumberalloted);
		}
	}

	printf("\n\n");
	//}//
	//else
	//	printf("\n\nUser doesn't exist\n\n");
		
}

void booktickets(int clisock,struct client c)
{
	//bookticket		
	struct bookticket b;
	int tr_num,seats;
	struct train tr;
		
	int size;
	read(clisock,&size,sizeof(size));	
	printf("TrainId\tTrain_name\tAvailable_seats\tLastAllotedSeat\n");
	while(size > 0)
	{
		read(clisock,&size,sizeof(size));	
		//printf("%d\n",size);
		read(clisock,&tr,sizeof(tr));		
		if(strcmp(tr.trainname,"removed") !=0)
			printf("%d\t%s\t%d\t\t%d\n",tr.train_id,tr.trainname,tr.seats,tr.lastseatalloted);
	}

	
	printf("\n\nProvide train detais, to book your tickets\nTrain number: ");
	scanf("%d",&b.train_id);
	
	printf("Seats : ");
	scanf("%d",&b.seatsreq);
	b.uid=c.userid;
	write(clisock,&c,sizeof(c));
	write(clisock,&b,sizeof(b));
	int bookstat;
	read(clisock,&bookstat,sizeof(bookstat));
	if(bookstat == -1)
	{
		//continue;
		printf("\n\nBooking error!!Either, number of seats requested exceeded availability, or train details provided to book tickets, is inappropriate.\n");
	}
	else
	{
		printf("\n\nSeat booking confirmed.Check your bookings for verification\n");
	}

}


void increase_or_decrease_seats(int clisock,struct client c)
{
	prevbookings(clisock,c);
	printf("\n\nWould you like to edit your booking?Please provide booking ID\n");
	int bid;
	scanf("%d",&bid);

	int fd=open("booking.txt",O_RDWR);
	struct flock f;
	f.l_type=F_WRLCK;
	f.l_len=0;
	f.l_start=0;
	f.l_whence=SEEK_SET;
	f.l_pid=getpid();
	fcntl(fd,F_SETLKW,&f);

	struct bookingDetails bd;
	lseek(fd,(bid-1)*sizeof(struct bookingDetails) , SEEK_SET);
	read(fd,&bd,sizeof(bd));

	f.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&f);
	close(fd);
	int seats;

	printf("\n\n1. Book more tickets for same details.\n2. Cancel the booking\n");
	
	int ch;
	scanf("%d",&ch);
	if(ch == 1)
	{

		printf("\nHow many seats you want to book further?\n");
		scanf("%d",&seats);
	}
	else 
		seats=0;

	struct bookticket b;
	b.train_id=bd.trainid;
	b.seatsreq=seats;
	b.uid=bd.userid;
	
	write(clisock,&b,sizeof(b));
	write(clisock,&bd.bookingid,sizeof(bd.bookingid));
	write(clisock,&ch,sizeof(ch));
	write(clisock,&c,sizeof(c));
	
	int stat;
	read(clisock,&stat,sizeof(stat));
	
	if(stat == -1)
		printf("\n\nCould not complete your request. Please verify the details entered, and make sure it's right and try again.\n");
	else
		printf("\n\nSuccessfully updated booking, as per your request\n");
}

void cancelbooking(int clisock,struct client c)
{
	prevbookings(clisock,c);
	printf("\nPlease enter booking Id, to cancel the ticket.\n");
	int bid;
	scanf("%d",&bid);
	write(clisock,&bid,sizeof(bid));
	write(clisock,&c.userid,sizeof(c.userid));
	write(clisock,&c,sizeof(c));

	int cstat;
	read(clisock,&cstat,sizeof(cstat));
	if(cstat == -1)
		printf("\n\nCould not cancel the given booking, maybe, because you entered wrong booking id, or the given booking wasn't intended to you. Please check your bookings and proceed again. Thank you\n");
	else
		printf("\n\nBooking cancelled.\n");
}


void usermenu(int clisock,struct client c)

{
	while(1)
	{
		printf("1. Book Ticket\n2. View previous Bookings\n3. Update Booking\n4. Cancel Booking\n5. Logout\n");
		int option;
		printf("\nYour choice : ");
		scanf("%d",&option);
		if(option<1 || option>5)
		{
			printf("Please enter valid choice\n");
			//exit(0);
		}
		else
		{
			write(clisock,&option,sizeof(int));
			if(option == 1)
			{
			//printf("cs=%d\n",clisock);
			booktickets(clisock,c);
			}
			else if(option == 2)
			{
			//printf("cs=%d\n",clisock);
			prevbookings(clisock,c);

			}	
			else if(option == 3)
			{
			//printf("cs=%d\n",clisock);
			//update ticket
			increase_or_decrease_seats(clisock,c);
			}
			else if(option == 4)
			{
			//printf("cs=%d\n",clisock);
			//cancel booking
			cancelbooking(clisock,c);
			}
			else if(option == 5)
			{
			//printf("cs=%d\n",clisock);
			close(clisock);
			exit(0);
			}
		}
	}
}


void ischoicelogin(int clientSocket)
{

	printf("Login as\n1. User\n2. Agent\n3. Admin\n");
	int type;
	scanf("%d",&type);

	if(type <1 || type >3)
	{
		printf("\n\nNo such user exists\n");
		//exit(0);
	}
	else
	{
		
		write(clientSocket,&type,sizeof(type));
		int userid,password;
		printf("Your userid ");
		scanf("%d",&userid);
		write(clientSocket,&userid,sizeof(userid));

		printf("Your password ");
		scanf("%d",&password);
			
		write(clientSocket,&password,sizeof(password));
		struct client cli;
		cli.userid=userid;
		cli.password=password;
		cli.type=type;
			
		int login_status;
		read(clientSocket,&login_status,sizeof(login_status));
		//printf("login stat = %d\n",login_status);

		if(login_status == 1)
		{
		if(cli.type == 1 || cli.type == 2)
			usermenu(clientSocket,cli);
		else
			adminmenu(clientSocket,cli);
		}	
		else if(login_status == 0)
		{
			printf("\nLogin failed!!\n");
		}
		else
		{
		printf("\n\nYou're already logged in from different terminal. Please exit out.So you can login here.\n\n");
		}
	}
			
}


void ischoiceregister(int clientSocket)
{
	int type,userid,password;
			

	printf("Enter the type of account :\n");
	printf("1. User\n2. Agent\n3. Admin\n");
	printf("Your Choice: ");
	scanf("%d", &type);
			

	if(type >3  || type < 1)
	{

		printf("Invalid Choice!\n");
		//exit(0);
	}

	else
	{
		//printf("CS=%d",clientSocket);
		write(clientSocket, &type, sizeof(type));
		//perror("");

		printf("Enter userid : ");
		scanf("%d",&userid);

	
		write(clientSocket, &userid, sizeof(userid));
		//perror("");

		printf("Enter password : ");

		scanf("%d",&password);

		write(clientSocket, &password, sizeof(password));

		//perror("");
	
		int regstat;

		read(clientSocket,&regstat,sizeof(regstat));


		if(regstat == 1)
		{
			printf("\n\nRegistered successfully, you can now login\n");
		}
		else
		{
			printf("\n\nCould not register,please try to change userid or password, and reregister, yourself.\n");
		}
	}
}

void mainmenu(int clientSocket)
{
	//while(1){
	printf("\n-------Welcome to Indian Railways------\n");	
	printf("Enter your choice\n1. Login\n2. New User?Register\n3. Exit\n");

	int choice;
	scanf("%d",&choice);
		
	if(choice == 3)
	{
		printf("\n\nThanks for  visiting . Have a good day\n\n");
		exit(0);
	}
	write(clientSocket,&choice,sizeof(choice));
	if(choice == 1)
	{
		ischoicelogin(clientSocket);
	}
	else if(choice == 2)
	{
		ischoiceregister(clientSocket);
	}
	else
	{
		printf("\n\nPlease enter valid choice\n");
	}
	
}


int main()
{

	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[1024];

	clientSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	if(clientSocket < 0){
		perror("");
		exit(1);
	}
	printf("Client Socket is created.\n");

	serverAddr.sin_family = AF_UNIX;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;//inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		perror("");
		exit(1);
	}
	printf("Connected to Server.\n");

	while(1)
	{
		mainmenu(clientSocket);
		
	}
	//mainmenu(clientSocket);
	close(clientSocket);
	return 0;
}
