/*
 *		SERVER SIDE PROGRAM FOR RAILWAY TICKET BOOKING SYSTEM
 */
#include"util.h"
#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<pthread.h>
#define PORT 4466

int login(int);
void register_user(int);

int menuforadmin(int);
void searchtrain(int);
void updatetrain(int);
void deletetrain(int);
void addtrain(int);
void deleteuser(int);
void adduserdetails(int );
void searchuserbyid(int );
void bookingtrainlist(int);

int menuforuser(int);
void viewPrevBookings(int );
void cancelbooking(int);
void booktickets(int);


void booktickets(int newSocket)
{
	int fd1=open("train.txt",O_RDWR);
	struct flock l;
	l.l_type=F_WRLCK;
	l.l_len=0;
	l.l_start=0;
	l.l_whence=SEEK_SET;
	l.l_pid=getpid();
	fcntl(fd1,F_SETLKW,&l);
	struct train t;
	int end=lseek(fd1,0,SEEK_END);
       	int size=end/sizeof(t);
	//printf("lseek %d\nsize of t %ld\n",end,sizeof(t));
	lseek(fd1,0,SEEK_SET);
	write(newSocket,&size,sizeof(size));
	while(size > 0)
	{
		size--;
		//printf("%d\n",size);
		read(fd1,&t,sizeof(t));
		//if(strcmp(t.trainname,"removed") != 0)
		//{
			write(newSocket,&size,sizeof(size));
			write(newSocket,&t,sizeof(t));
		//}
	}

	l.l_type=F_UNLCK;
	fcntl(fd1,F_SETLK,&l);
	close(fd1);

	struct bookticket b;
	struct client c1;
	read(newSocket,&c1,sizeof(c1));
	read(newSocket,&b,sizeof(b));
	int bookingstatus=BookTicket(b,c1);
	if(bookingstatus == -1)
	{
		write(newSocket,&bookingstatus,sizeof(bookingstatus));
	}
	else
	{
		write(newSocket,&bookingstatus,sizeof(bookingstatus));
	}

}

void deletetrain(int socket)
{
	int trainid;
	read(socket,&trainid,sizeof(trainid));
	int stat=deleteTrain(trainid);
	write(socket,&stat,sizeof(stat));
}
void addtrain(int socket)
{
	struct train tnew;
	read(socket,&tnew,sizeof(tnew));
	struct train stat=addTrain(tnew);
	write(socket,&stat,sizeof(stat));
}
void updatetrain(int newSocket)
{
	int fd1=open("train.txt",O_RDWR);
	struct flock l;
	l.l_type=F_WRLCK;
	l.l_len=0;
	l.l_start=0;
	l.l_whence=SEEK_SET;
	l.l_pid=getpid();
	fcntl(fd1,F_SETLKW,&l);
	struct train t;
	int end=lseek(fd1,0,SEEK_END);
       	int size=end/sizeof(t);
	//printf("lseek %d\nsize of t %ld\n",end,sizeof(t));
	lseek(fd1,0,SEEK_SET);
	write(newSocket,&size,sizeof(size));
	while(size > 0)
	{
		size--;
		//printf("%d\n",size);
		read(fd1,&t,sizeof(t));
		//if(strcmp(t.trainname,"removed") != 0)
		//{
			write(newSocket,&size,sizeof(size));
			write(newSocket,&t,sizeof(t));
		//}
	}
	l.l_type=F_UNLCK;
	fcntl(fd1,F_SETLK,&l);
	close(fd1);	
		
		
	int trainid,seats,increased;
	read(newSocket,&trainid,sizeof(trainid));
	read(newSocket,&seats,sizeof(seats));
	read(newSocket,&increased,sizeof(increased));
	int stat=updateSeats(trainid,seats,increased);
	write(newSocket,&stat,sizeof(stat));
}

void searchtrain(int newSocket)
{
	int trainid;
	read(newSocket,&trainid,sizeof(trainid));
	struct train stat=searchTrainByName(trainid);

	write(newSocket,&stat,sizeof(stat));		
}
void deleteuser(int newSocket)
{
	int uid;
	struct client c;
	read(newSocket,&uid,sizeof(uid));
	read(newSocket,&c,sizeof(c));
	int u=deleteUser(uid,c);
	write(newSocket,&u,sizeof(u));
}
void adduserdetails(int sock)
{
	struct client c;
	read(sock, &c.type, sizeof(c.type));
	read(sock, &c.userid, sizeof(c.userid));
	read(sock, &c.password, sizeof(c.password));
	c.active=0;
	if(addUser(c) == 1)
	{
	//	printf("Registered successfull\n");
		int reg=1;
		write(sock,&reg,sizeof(reg));
	}
	else
	{
	//	printf("unregisterd\n");
		int reg=0;
		write(sock,&reg,sizeof(reg));
	}

}
void searchuserbyid(int newSocket)
{
	int uid;
	read(newSocket,&uid,sizeof(uid));
	searchUser(newSocket,uid);
}
void bookingtrainlist(int newSocket)
{
	int fd1=open("train.txt",O_RDWR);
        struct flock l;
        l.l_type=F_WRLCK;
        l.l_len=0;
        l.l_start=0;
        l.l_whence=SEEK_SET;
        l.l_pid=getpid();
        fcntl(fd1,F_SETLKW,&l);
        struct train t;
        int end=lseek(fd1,0,SEEK_END);
        int size=end/sizeof(t);
        //printf("lseek %d\nsize of t %ld\n",end,sizeof(t));
        lseek(fd1,0,SEEK_SET);
        write(newSocket,&size,sizeof(size));
        while(size > 0)
        {
                size--;
          //      printf("%d\n",size);
                read(fd1,&t,sizeof(t));
                write(newSocket,&t,sizeof(t));
                write(newSocket,&size,sizeof(size));
        }

        l.l_type=F_UNLCK;
        fcntl(fd1,F_SETLK,&l);
        close(fd1);
	int tid;
	read(newSocket,&tid,sizeof(tid));
	TrainBookings(tid,newSocket);

}
int menuforadmin(int newSocket)
{
	while(1)
	{
	int furtherchoice;
	read(newSocket,&furtherchoice,sizeof(furtherchoice));
	if(furtherchoice == 1)
	{
		addtrain(newSocket);
	}
	else if(furtherchoice == 2)
	{
		deletetrain(newSocket);
	}
	else if(furtherchoice == 3)
	{
		updatetrain(newSocket);
		
	}
	else if(furtherchoice == 4)
	{
		searchtrain(newSocket);
	}
	else if(furtherchoice == 5)
	{
		deleteuser(newSocket);
	}
	else if(furtherchoice == 6)
	{

		adduserdetails(newSocket);
	}
	else if(furtherchoice == 7)
	{
		searchuserbyid(newSocket);
	}
	else if(furtherchoice == 8)
	{
		bookingtrainlist(newSocket);
	}
	else if(furtherchoice == 9)
	{
		printf("\n---ADMIN LOGGED OFF---\n");
		break;
	}
	}
	close(newSocket);
	//printf("--------ADMIN LOGGED OFF----------\n");
	return -1;
}

void viewPrevBookings(int socket)
{
	//printf("s=%d",socket);
	struct client c;
	read(socket,&c,sizeof(c));
	//perror("");
	//printf("userid= %d\n",c.userid);
	//perror("");
	PrevBookings(c,socket);
}

void updatebooking(int socket)
{
	//printf("s=%d\n",socket);
	viewPrevBookings(socket);
	struct bookticket b;
	read(socket,&b,sizeof(b));
	//printf("after reading b struct\n");
	//perror("");
	int bid;
	read(socket,&bid,sizeof(bid));
	
	//printf("after reading bid\n");
	//perror("");
	int ch;
	read(socket,&ch,sizeof(ch));
	struct client c1;
	read(socket,&c1,sizeof(c1));
	//printf("after reading  choice\n");
	int upstat=updateBookings(b,bid,ch,c1);
	write(socket,&upstat,sizeof(upstat));
}
void cancelbooking(int socket)
{
	viewPrevBookings(socket);
	int bid,uid;
	read(socket,&bid,sizeof(bid));
	read(socket,&uid,sizeof(uid));
	struct client c1;
	read(socket,&c1,sizeof(c1));
	int cstat=cancelBookings(bid,uid,c1);
	write(socket,&cstat,sizeof(cstat));
}

int menuforuser(int newSocket)
{
	while(1)
	{
	int furtherchoice;
	read(newSocket,&furtherchoice,sizeof(furtherchoice));
	if(furtherchoice == 1)
	{
	//	printf("sock=%d\n",newSocket);
		booktickets(newSocket);
	}
	else if(furtherchoice == 2)
	{

	//	printf("sock=%d\n",newSocket);
		viewPrevBookings(newSocket);
	}
	else if(furtherchoice == 3)
	{
	//	printf("sock=%d\n",newSocket);
		updatebooking(newSocket);
		//UpdateBooking, write code to update numb of seats.
	}
	else if(furtherchoice == 4)
	{
	//	printf("sock=%d\n",newSocket);
		cancelbooking(newSocket);
	}
	else if(furtherchoice == 5)
	{
		//logout
		printf("\n---USER LOGGED OFF---\n");
		//close(newSocket);
		break;
	}
	}
	close(newSocket);
	
	return -1;
}

void * start(void *args)
{
	int *ptr=(int *)args;
	int newSocket=*ptr;
	while(1)
	{
		int choice,loginstat=0;
		read(newSocket,&choice,sizeof(choice));
		if(choice == 1)
		{
			int r=login(newSocket);
		        if(r == -1)
			{

				break;
			}
		}
		else if(choice == 2)
		{
			//int regstat=0;i
			register_user(newSocket);// != 0)
		}
		else if(choice == 3)
		{
			//close(newSocket);
			break;
		}
	}
	close(newSocket);

}

int login(int socket)
{
	struct client c;
	
	read(socket,&c.type,sizeof(c.type));
	//perror("");
	read(socket,&c.userid,sizeof(c.userid));
	//perror("");
	read(socket,&c.password,sizeof(c.password));
	//perror("");

	//verify credentials
	int fd=open("account.txt",O_RDWR);
        
	struct flock l;
        l.l_type=F_WRLCK;
        l.l_len=0;
        l.l_start=0;
        l.l_whence=SEEK_SET;
        l.l_pid=getpid();
        
	fcntl(fd,F_SETLKW,&l);
	int cnt=0;

        int flag=0;
        struct client cli;
        while(read(fd, &cli, sizeof(cli))>0)
	{
		//printf("iI\n");
		cnt++;
                if(c.userid == cli.userid && c.type == cli.type && c.password == cli.password)
                {
			//printf("found\n");
			//printf("input\n%d\t%d\t%d\t%d\t",c.userid,c.password,c.type,c.active);
			//printf("matchedrec\n%d\t%d\t%d\t%d\t",cli.userid,cli.password,cli.type,cli.active);
			if(cli.active == 0)
			{
				//printf("entered\n");
				flag=1;
				if(cli.type == 1 || cli.type == 3)
				{
					//printf("entered\n");
					cli.active=1;//make user active
					lseek(fd,-1*sizeof(cli),SEEK_CUR);	
					write(fd,&cli,sizeof(cli));
				}
			}
			else
			{
				flag=-1;
				//printf("entered ter\n");
			}
                        break;
                }
        }

        l.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&l);
	close(fd);

	write(socket,&flag,sizeof(flag));
	//printf("flag = %d\n",flag);	
	if(flag== 1)
	{
		//printf("Loginsuccessfull\n");
		if(c.type == 1 || c.type == 2)
		{
			if(menuforuser(socket) == -1)
			{
				fd=open("account.txt",O_RDWR);
        
				struct flock l;
        			l.l_type=F_WRLCK;
        			l.l_len=0;
        			l.l_start=0;
        			l.l_whence=SEEK_SET;
				l.l_pid=getpid();
				fcntl(fd,F_SETLKW,&l);
				//printf("Inside cs , cnt= %d\n",cnt);	
				lseek(fd,(cnt-1)*sizeof(cli),SEEK_SET);
				//perror("");
				read(fd,&cli,sizeof(cli));
				//perror("");

				cli.active=0;//make user active
				lseek(fd,-1*sizeof(cli),SEEK_CUR);	
				//perror("");
				write(fd,&cli,sizeof(cli));
				//perror("");
				l.l_type=F_UNLCK;
				fcntl(fd,F_SETLK,&l);


				//printf("Afterlogout\n");
				close(socket);
				return -1;
			}	
		}
		else
		{
			if(menuforadmin(socket) == -1)
			{
				fd=open("account.txt",O_RDWR);
        
				struct flock l;
        			l.l_type=F_WRLCK;
        			l.l_len=0;
        			l.l_start=0;
        			l.l_whence=SEEK_SET;
				l.l_pid=getpid();
				fcntl(fd,F_SETLKW,&l);
				//printf("Inside cs , cnt= %d\n",cnt);	
				lseek(fd,(cnt-1)*sizeof(cli),SEEK_SET);
				//perror("");
				read(fd,&cli,sizeof(cli));
				//perror("");
				cli.active=0;//make user active
				lseek(fd,-1*sizeof(cli),SEEK_CUR);	
				//perror("");
				write(fd,&cli,sizeof(cli));
				//perror("");
				l.l_type=F_UNLCK;
				fcntl(fd,F_SETLK,&l);

				close(socket);
				return -1;
			}
	
		}
	}
	else
	{
		//printf("Login unsucessfull\n");
	}
	
}

void register_user(int sock){
	struct client c;

	read(sock, &c.type, sizeof(c.type));
	read(sock, &c.userid, sizeof(c.userid));
	read(sock, &c.password, sizeof(c.password));
	c.active=0;
	int stat=addUser(c);
	write(sock,&stat,sizeof(stat));
	if(stat == 1)
	{
		printf("Registered successfull\n");
	
	}
	else
	{
		printf("Unregistered\n");
		//return 0;
	}
}


int main(){

	int sockfd, ret;
	 struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buffer[1024];
	pid_t childpid;

	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("Server Socket is created.\n");

	serverAddr.sin_family = AF_UNIX;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr =INADDR_ANY;// inet_addr("127.0.0.1");

	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		perror("");
		exit(1);
	}

	if(listen(sockfd, 3) == 0){
		printf("Listening....\n");
	}else{
		printf("Error in binding.\n");
	}


	while(1){
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0){
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
		pthread_t thread;
		pthread_create(&thread,NULL,start,&newSocket);
		//pthread_join(thread,NULL);
		}
}
