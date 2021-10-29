/*
 *	UTILITIES FOR RAILWAY TICKET BOOKING SYSTEM
 */
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

/*	------STRUCTURE FOR DATABASE RECORDS------ */

struct bookticket
{
	int train_id;
	int seatsreq;
	int uid;
};

struct bookingDetails{
	int bookingid;
	int userid;
	int trainid;
	char trainname[20];
	char bookingstatus;
	int seatnumberalloted;
};

struct client
{
	int userid;
	int password;
	int type;
	int active;
};

struct train
{
	int train_id;
	char trainname[20];
	int seats;
	int lastseatalloted;
};
struct vacancy
{
	int trainid;
	int seatnumber;
};


void TrainBookings(int ,int );
int deleteUser(int,struct client);
int updateSeats(int,int,int);
struct train searchTrainByName(int);
struct train addTrain(struct train);
int addUser(struct client);
int deleteTrain(int);
int BookTicket(struct bookticket ,struct client);
int cancelBookings(int,int,struct client);
int updateBookings(struct bookticket ,int ,int ,struct client );
void searchUser(int ,int );
int isValidBooking(int ,int);
void PrevBookings(struct client,int);
int isValidTrain(int);
int isUserValid(struct client);


int BookTicket(struct bookticket b,struct client c)
{
	if(isUserValid(c) == -1)
		return -1;
	int flag=0;
	struct train t;
	int fd=open("train.txt",O_RDWR);
	struct flock l;
	l.l_type=F_WRLCK;
	l.l_len=0;
	l.l_start=0;
	l.l_whence=SEEK_SET;
	l.l_pid=getpid();
	fcntl(fd,F_SETLKW,&l);

	while(read(fd, &t, sizeof(t))>0)
	{
		if(t.train_id == b.train_id)
		{
			if(t.seats-b.seatsreq >= 0)
			{
				flag=1;
			}
			break;
		}
	}

	if(flag == 0)
	{
		l.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&l);
		close(fd);
		return -1;
	}
	else
	{
		
		//train file update
		int last;
		lseek(fd,-sizeof(struct train),SEEK_CUR);
		
		t.seats=t.seats-b.seatsreq;
		last=t.lastseatalloted;

		//add booking details to booking file
		int fd1=open("booking.txt",O_RDWR|O_APPEND);
		struct flock l1;
		l1.l_type=F_WRLCK;
		l1.l_len=0;
		l1.l_start=0;
		l1.l_whence=SEEK_SET;
		l1.l_pid=getpid();
		fcntl(fd1,F_SETLKW,&l1);
		
		int s=b.seatsreq;
		int fd3=open("vacancies.txt",O_RDWR,O_APPEND);
		//printf("fd3= %d\n",fd3);
		struct flock l2;
		l2.l_type=F_WRLCK;
		l2.l_len=0;
		l2.l_start=0;
		l2.l_whence=SEEK_SET;
		l2.l_pid=getpid();
		fcntl(fd3,F_SETLKW,&l2);
		//perror("");

		struct vacancy v;	


		int seatava=-1;
		while(read(fd3,&v,sizeof(v)) > 0)
		{
			if(v.trainid == b.train_id)
			{
				//printf("found vacancy\n");
				seatava=v.seatnumber;
				break;
			}
		}

		while(s >0)
		{
			s--;
			struct bookingDetails bd;
			int start=lseek(fd1,0,SEEK_SET);
        		int end=lseek(fd1,0,SEEK_END);
			if((end-start) == 0)
                		bd.bookingid=1;
			else
			{
				lseek(fd1,-1*sizeof(struct bookingDetails),SEEK_CUR);
                		struct bookingDetails temp;
				read(fd1,&temp,sizeof(temp));
                		bd.bookingid=temp.bookingid+1;
			}
				
			bd.userid=b.uid;
			bd.trainid=b.train_id;
			strcpy(bd.trainname,t.trainname);
			bd.bookingstatus='C';
			//if(i>=0 && arr[i] != -1)
			if(seatava != -1)
			{
				//printf("sear %d\n",seatava);
				bd.seatnumberalloted=seatava;
				seatava=-1;
				lseek(fd3,-1*sizeof(v),SEEK_CUR);
				read(fd3,&v,sizeof(v));
				//perror("");	
				v.trainid=-1;
				lseek(fd3,-1*sizeof(v),SEEK_CUR);
				//perror("");	
				write(fd3,&v,sizeof(v));
				//perror("");	
				while(read(fd3,&v,sizeof(v)) > 0)
				{
					if(v.trainid == b.train_id)
					{
						//printf("found vacancy\n");
						seatava=v.seatnumber;
						break;
					}
				}
			}
			
			else
			{
				//bd.seatnumberalloted=++t.lastseatalloted;
				bd.seatnumberalloted=++last;
				t.lastseatalloted=t.lastseatalloted+1;
			}

			write(fd1,&bd,sizeof(bd));
			//printf("Inside bookk\n");
			//perror("");
			//printf("Outside bookk\n");
		}	
		l2.l_type=F_UNLCK;
		fcntl(fd3,F_SETLK,&l2);
		close(fd3);

		write(fd,&t,sizeof(t));
		l.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&l);
		close(fd);
		
		l1.l_type=F_UNLCK;
		fcntl(fd1,F_SETLK,&l1);
		close(fd1);

		return 1;
	}
	
}

int isUserValid(struct client cli)
{
	int fd=open("account.txt",O_RDWR|O_APPEND);
	//add
	struct flock l;
	l.l_type=F_WRLCK;
	l.l_len=0;
	l.l_start=0;
	l.l_whence=SEEK_SET;
	l.l_pid=getpid();
	fcntl(fd,F_SETLKW,&l);
	if(cli.userid == -1)
		return -1;	
	int flag=0;
	struct client c;
	while(read(fd, &c, sizeof(c))>0)
	{
		if( c.userid == cli.userid && c.type == cli.type && c.password == cli.password)
		{
			flag=1;
			break;
		}
	}

	if(flag == 0)
	{
		return -1;
	}
	else
	{
		return 1;
	}

}

int addUser(struct client cli)
{
	int fd=open("account.txt",O_RDWR|O_APPEND);
	//add
	struct flock l;
	l.l_type=F_WRLCK;
	l.l_len=0;
	l.l_start=0;
	l.l_whence=SEEK_SET;
	l.l_pid=getpid();
	
	fcntl(fd,F_SETLKW,&l);
	int flag=0;
	struct client c;
	while(read(fd, &c, sizeof(c))>0)
	{
		if( c.userid == cli.userid)
		{
			flag=1;
			break;
		}
	}

	if(flag == 0)
	{
		//add use
		write(fd,&cli,sizeof(cli));
		l.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&l);
		close(fd);
		return 1;
	}
	else
	{
		l.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&l);
		close(fd);
		//userid exists
		return -1;
	}

}

int deleteTrain(int trainid)
{
	if(isValidTrain(trainid) == 1)
	{
		//remove all bookings for given train
		int fd=open("booking.txt",O_RDWR);
		struct flock l;
		l.l_type=F_RDLCK;
		l.l_len=0;
		l.l_start=0;
		l.l_whence=SEEK_SET;
		l.l_pid=getpid();
		fcntl(fd,F_SETLKW,&l);
		struct bookingDetails bd;
		while(read(fd,&bd,sizeof(bd)) > 0)
		{
			if(bd.trainid == trainid)
			{
				bd.bookingstatus='X';
				bd.seatnumberalloted=-1;
				lseek(fd,-1*sizeof(bd),SEEK_CUR);
				write(fd,&bd,sizeof(bd));
			}

		}
		l.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&l);
		
		//delete train
		
		fd=open("train.txt",O_RDWR);
		//perror("");
		//printf("fd=%d\n",fd);
		//struct flock l;
		l.l_type=F_WRLCK;
		l.l_len=0;
		l.l_start=0;
		l.l_whence=SEEK_SET;
		l.l_pid=getpid();
		fcntl(fd,F_SETLKW,&l);
		struct train t;
		int m=lseek(fd,(trainid-1)*sizeof(struct train),SEEK_SET);
		//printf("recordmoved %d\n",m);
		read(fd,&t,sizeof(t));
		//printf("rec read= %s\t%d\t%d\n",t.trainname,t.train_id,t.seats);	
		if(strcmp(t.trainname,"removed")!=0 && t.train_id == trainid)
		{
			strcpy(t.trainname,"removed");
			t.seats=-1;
			t.lastseatalloted=-1;
			int r=lseek(fd,-1*sizeof(t),SEEK_CUR);
			//printf("After moving and getting curr  rec %d\n",r);
			write(fd,&t,sizeof(t));
			//perror("");
			l.l_type=F_UNLCK;
			fcntl(fd,F_SETLK,&l);
			return 1;
		}
		else
		{
			l.l_type=F_UNLCK;
			fcntl(fd,F_SETLK,&l);
			return -1;
		}

	}
	else return -1;

}

int isValidTrain(int trainid)
{
	struct train t;
	int fd=open("train.txt",O_RDWR);
	struct flock l;
	l.l_type=F_WRLCK;
	l.l_len=0;
	l.l_start=0;
	l.l_whence=SEEK_SET;
	l.l_pid=getpid();
	fcntl(fd,F_SETLKW,&l);
	int flag=0;
	while(read(fd,&t,sizeof(t)) >0)
	{
		if(t.train_id == trainid)
		{
			if(strcmp(t.trainname,"removed")!=0)
			{
				flag=1;	
				break;
			}
			else
			{
				break;
			}
		}
	}
	l.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&l);
	close(fd);
	if(flag == 1)
		return 1;
	else
		return 0;
}
struct train addTrain(struct train t)
{
	int fd=open("train.txt",O_RDWR|O_APPEND);
	//perror("");
	//printf("fd=%d\n",fd);
	struct flock l;
	l.l_type=F_WRLCK;
	l.l_len=0;
	l.l_start=0;
	l.l_whence=SEEK_SET;
	l.l_pid=getpid();
	fcntl(fd,F_SETLKW,&l);
	//add
	struct train temp;
	int start=lseek(fd,0,SEEK_SET);
	int end=lseek(fd,0,SEEK_END);
	if((end-start) == 0)
		t.train_id=1;
	else
	{
		lseek(fd,-1*sizeof(struct train),SEEK_CUR);
		read(fd,&temp,sizeof(temp));
		t.train_id=temp.train_id+1;

	}
	int w=write(fd,&t,sizeof(t));
	//printf("w=%d\ninside util\n",w);
	//perror("");
	//printf("outside util\n");
	l.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&l);
	close(fd);
	return t;
}

int updateSeats(int trainid,int seats,int increased)
{
	if(isValidTrain(trainid) == 1)
	{
		int fd=open("train.txt",O_RDWR);
		//perror("");
		//printf("fd=%d\n",fd);
		struct flock l;
		l.l_type=F_WRLCK;
		l.l_len=0;
		l.l_start=0;
		l.l_whence=SEEK_SET;
		l.l_pid=getpid();
		fcntl(fd,F_SETLKW,&l);
		struct train t;
		int m=lseek(fd,(trainid-1)*sizeof(struct train),SEEK_SET);
		//printf("recordmoved %d\n",m);
		read(fd,&t,sizeof(t));
		//printf("rec read= %s\t%d\t%d\n",t.trainname,t.train_id,t.seats);	
		if(strcmp(t.trainname,"removed")!=0 && t.train_id == trainid)
		{
			if(increased == 1)
				t.seats+=seats;
			else
			{
				
				t.seats-=seats;
				if(t.seats < 0)
				{
					return -1;
				}
				if(t.seats < t.lastseatalloted)
				{
					t.lastseatalloted=t.seats;
					int gd=open("booking.txt",O_RDWR);
					struct flock f;
					f.l_type=F_WRLCK;
					f.l_len=0;
					f.l_start=0;
					f.l_whence=SEEK_SET;
					f.l_pid=getpid();
					fcntl(gd,F_SETLKW,&f);
					struct bookingDetails r;

					while(read(gd,&r,sizeof(r)) >0)
					{
						if(r.trainid == trainid && r.seatnumberalloted >t.seats)
						{
							lseek(gd,-1*sizeof(r),SEEK_CUR);
							r.seatnumberalloted=-1;
							r.bookingstatus='X';
							write(gd,&r,sizeof(r));
						}	
					}

					f.l_type=F_UNLCK;
					fcntl(gd,F_SETLK,&f);
					close(gd);
				}
			}
			int r=lseek(fd,-1*sizeof(t),SEEK_CUR);
			//printf("After moving and getting curr  rec %d\n",r);
			write(fd,&t,sizeof(t));
			//perror("");
			l.l_type=F_UNLCK;
			fcntl(fd,F_SETLK,&l);
			return 1;
		}
		else
		{
			l.l_type=F_UNLCK;
			fcntl(fd,F_SETLK,&l);
			return -1;
		}

		
	}
	else 
		return -1;

}


struct train searchTrainByName(int trainid)
{
		int fd=open("train.txt",O_RDWR);
		struct flock l;
		l.l_type=F_WRLCK;
		l.l_len=0;
		l.l_start=0;
		l.l_whence=SEEK_SET;
		l.l_pid=getpid();
		fcntl(fd,F_SETLKW,&l);
		struct train t;
		int found=0;
		while(read(fd,&t,sizeof(t)) >0)
		{
			if(t.train_id == trainid && strcmp(t.trainname,"removed") !=0 )
			{
				found=1;
				break;
			}
		}
		l.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&l);
		close(fd);
		if(found == 1)
			return t;
		else
		{
			struct train t;
			t.train_id = -1;
			return t;
		}
}
int deleteUser(int uid,struct client c1)
{
	
	int fd=open("account.txt",O_RDWR);
	
	int flag=0;
	struct client c;
	struct flock l;
	l.l_type=F_WRLCK;
	l.l_len=0;
	l.l_start=0;
	l.l_whence=SEEK_SET;
	l.l_pid=getpid();
	fcntl(fd,F_SETLKW,&l);
	
	while(read(fd, &c, sizeof(c))>0)
	{
		if( c.userid == uid)
		{
			flag=1;
			break;
		}
	}

	if(flag == 0)
	{
		l.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&l);
		return -1;
	}
	else
	{
		//perror("");
		int fd1=open("booking.txt",O_RDWR);
		struct flock f;
		f.l_type=F_WRLCK;
		f.l_len=0;
		f.l_start=0;
		f.l_whence=SEEK_SET;
		f.l_pid=getpid();
		fcntl(fd1,F_SETLK,&f);
		
		struct bookingDetails h;
		while(read(fd1,&h,sizeof(h)) >0)
		{
			if(h.userid == uid)
			{
				cancelBookings(h.bookingid,uid,c1);
			}
		}
		
		f.l_type=F_UNLCK;
		fcntl(fd1,F_SETLK,&f);
		close(fd1);
		
		c.userid=-1;
		lseek(fd,-1*sizeof(c),SEEK_CUR);
		write(fd,&c,sizeof(c));

		l.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&l);
		close(fd);
		return 1;
	}

}

void PrevBookings(struct client c,int socket)
{
	//if(isUserValid(c)= -1)
	
	//printf("socket = %d\n",socket);
	int fd=open("booking.txt",O_RDWR);
	
	struct flock l;
	l.l_type=F_WRLCK;
	l.l_len=0;
	l.l_start=0;
	l.l_whence=SEEK_SET;
	l.l_pid=getpid();
	fcntl(fd,F_SETLKW,&l);
	
	int length=lseek(fd,0,SEEK_END)/sizeof(struct bookingDetails);
	lseek(fd,0,SEEK_SET);
	//printf("length of file %d\n",length);
	
	struct bookingDetails bd;
	int i=0;
	struct bookingDetails res[length];
		
	write(socket,&length,sizeof(length));
	//perror("");
	while(length >0)
	{
		length--;
		write(socket,&length,sizeof(length));
		read(fd,&bd,sizeof(bd));
		//if(bd.userid == c.userid)
		//{
                	write(socket,&bd,sizeof(bd));
			//res[i++]=bd;
		//}
	}

	l.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&l);
	close(fd);
}


int isValidBooking(int bid,int cid)
{
	int fd=open("booking.txt",O_RDWR);

	struct flock l;
	l.l_type=F_RDLCK;
	l.l_len=0;
	l.l_start=0;
	l.l_whence=SEEK_SET;
	l.l_pid=getpid();
	fcntl(fd,F_SETLKW,&l);
	struct bookingDetails bd;
	int flag=0;
	while(read(fd,&bd,sizeof(bd)) >0)
	{
		if(bd.bookingid == bid && bd.userid == cid && bd.bookingstatus == 'C')
		{
			if(isValidTrain(bd.trainid) == 1)
			{
				flag=1;
			}

			break;
		}
	}
	l.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&l);

	if(flag == 1)
		return 1;
	else
		return -1;

}

int cancelBookings(int bid,int uid,struct client t)
{
	if(isValidBooking(bid,uid) == 1 && isUserValid(t)!=-1)
	{
		int fd=open("booking.txt",O_RDWR);
		struct flock l;
		l.l_type=F_WRLCK;
		l.l_len=0;
		l.l_start=0;
		l.l_whence=SEEK_SET;
		l.l_pid=getpid();
		fcntl(fd,F_SETLKW,&l);

	
		struct bookingDetails bd;
		lseek(fd,(bid-1)*sizeof(bd),SEEK_SET);
		read(fd,&bd,sizeof(bd));
		lseek(fd,(-1)*sizeof(bd),SEEK_CUR);
		int deleted=bd.seatnumberalloted;
		bd.bookingstatus ='X';
		bd.seatnumberalloted=-1;
		//printf("del seat = %d\ntrain id = %d\n",deleted,bd.trainid);
		write(fd,&bd,sizeof(bd));
		
		l.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&l);
		close(fd);
		
		fd=open("train.txt",O_RDWR);
		//struct flock l;
		l.l_type=F_WRLCK;
		l.l_len=0;
		l.l_start=0;
		l.l_whence=SEEK_SET;
		l.l_pid=getpid();
		fcntl(fd,F_SETLKW,&l);
		struct train tr;

		//lseek(fd,(bd.train_id - 1)*sizeof(tr),SEEK_SET);
		while(read(fd,&tr,sizeof(tr)) >0)
		{
			if(tr.train_id == bd.trainid)
			{
				tr.seats=tr.seats+1;
				lseek(fd,-1*sizeof(tr),SEEK_CUR);
				write(fd,&tr,sizeof(tr));
				//printf("updated trainn\n");
				break;
			}
		}
		
		l.l_type=F_UNLCK;
		fcntl(fd,F_SETLK,&l);
		close(fd);

		fd=open("vacancies.txt",O_RDWR|O_APPEND);
		//perror("");
		//printf("fd=%d\nentering vacancy\n",fd);
		l.l_type=F_WRLCK;
		l.l_len=0;
		l.l_start=0;
		l.l_whence=SEEK_SET;
		l.l_pid=getpid();
		
		fcntl(fd,F_SETLKW,&l);
		
		struct vacancy v;
		v.trainid=bd.trainid;
		v.seatnumber=deleted;
		//printf("entering into file %d %d\n",v.trainid,v.seatnumber);

		write(fd,&v,sizeof(v));
		
		//printf("after writing\n");
		//perror("");
		//lseek(fd,0,SEEK_SET);
		//struct vacancy c;
		//read(fd,c,
		
		l.l_type=F_UNLCK;

		fcntl(fd,F_SETLK,&l);
		return 1;
	}
	else
		return -1;
}

void searchUser(int newSocket,int uid)
{
	
	int fd=open("account.txt",O_RDWR);
	struct flock l;
	l.l_type=F_RDLCK;
	l.l_len=0;
	l.l_start=0;
	l.l_whence=SEEK_SET;
	l.l_pid=getpid();
	fcntl(fd,F_SETLKW,&l);
	
	struct client c;
	int flag=0;
	while(read(fd,&c,sizeof(c)) > 0)
	{
		if(uid!= -1 && c.userid == uid)
		{
			flag=1;
			break;
		}
	}
	if(flag == 0)
		write(newSocket,&flag,sizeof(flag));
	else
	{

		write(newSocket,&flag,sizeof(flag));
		write(newSocket,&c,sizeof(c));
		//PrevBookings(c,newSocket);
	}


}

int updateBookings(struct bookticket b,int bid,int ch,struct client c)
{
	if(ch == 1)
	{
		if(isValidBooking(bid,b.uid))
			return BookTicket(b,c);
		else
			return -1;
	}
	else
		return cancelBookings(bid,b.uid,c);
}


void TrainBookings(int tid,int socket)
{
	//printf("socket = %d\n",socket);
	int fd=open("booking.txt",O_RDWR);
	
	struct flock l;
	l.l_type=F_WRLCK;
	l.l_len=0;
	l.l_start=0;
	l.l_whence=SEEK_SET;
	l.l_pid=getpid();
	fcntl(fd,F_SETLKW,&l);
	
	int length=lseek(fd,0,SEEK_END)/sizeof(struct bookingDetails);
	lseek(fd,0,SEEK_SET);
	//printf("length of file %d\n",length);
	
	struct bookingDetails bd;
	int i=0;
	struct bookingDetails res[length];
		
	write(socket,&length,sizeof(length));
	//perror("");
	while(length >0)
	{
		length--;
		write(socket,&length,sizeof(length));
		read(fd,&bd,sizeof(bd));
                write(socket,&bd,sizeof(bd));
	}

	l.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&l);
	close(fd);
}
