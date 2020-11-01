#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include <time.h>
#include "pokemon_info.h"
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#define STR_SIZE 20
#define MAP_WIDTH 10
#define MAP_HEIGHT 10
#define BUF_SIZE 1024

int _getch(void);
void getMonsters(user_info* user, M_Info *monsters);
void skill(user_info *user, M_Info monsters, int num);
void calculate_LV(user_info *user, int i);
void fight(user_info *user, M_Info *monsters);
void champ(user_info *user, M_Info *monsters);

int map[MAP_HEIGHT][MAP_WIDTH] = {
	  {2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	  {1, 1, 1, 1, 1, 0, 0, 0, 1, 0},
	  {0, 0, 1, 1, 1, 0, 0, 0, 1, 0},
	  {0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
	  {0, 1, 0, 0, 1, 0, 0, 1, 1, 1},
	  {1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
	  {0, 1, 1, 1, 1, 0, 0, 0, 0, 0},
	  {0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
	  {0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
	  {1, 1, 1, 1, 0, 0, 0, 1, 1, 3}
};


void Initialize(user_info* user, M_Info *monsters);
void load(user_info* user, M_Info *monsters);
void draw(int, int);
void show_userinfo(user_info* user);
int main()
{
	user_info user;
	M_Info monsters[10];

	int key;
	int x = MAP_WIDTH / 2;
	int y = MAP_HEIGHT / 2;
	int move_flag = 0;
	int battle;
	int choose;	//포켓몬 센터 선택 변수
	int status;
        char k;
	int n; //전투에 내보낼 포켓몬 선택 변수
	pid_t childpid;//fork 프로세스 구분 변수
        
        printf("신규 유저입니까? y/n\n");
        scanf("%c", &k);
        
        if(k=='n')
                load(&user, monsters);
        else if(k=='y')
                Initialize(&user, monsters);

	while (1)
	{
		key = _getch();
		printf("input2 = %d\n", key);

		switch (key)
		{
		case 65://위쪽 
			if (y == 0)
				break;
			if (map[y - 1][x] == 1)
				break;
			y--;
			break;
		case 68://왼쪽 
			if (x == 0)
				break;
			if (map[y][x - 1] == 1)
				break;
			x--;
			break;
		case 67://오른쪽 
			if (x == MAP_WIDTH - 1)
				break;
			if (map[y][x + 1] == 1)
				break;
			x++;
			break;
		case 66://아래쪽 
			if (y == MAP_HEIGHT - 1)
				break;
			if (map[y + 1][x] == 1)
				break;
			y++;
			break;
		default://이상한 입력일 때 
			break;
		}
		
		key = 0;

		draw(x, y);

		//랜덤 
		if (x == 0 && y == 0)
		{
			printf("둘 중 하나를 골라주세요. \n");//포켓몬 센터 (fork) 
			printf("포켓몬을 치료한다(1을 입력) \n");
			printf("몬스터볼을 구매한다(2을 입력)\n");
                        printf("진행사항을 저장한다(3을 입력)\n");
			choose = _getch();
			switch (choose) {

			case '1': //포켓몬 치료fork
				childpid = fork();
				if (childpid == -1) {
					perror("Failed to fork");
					return 1;
				}
				if (childpid)//부모 프로세스
				{
					wait(&status);
					cure(&user, 3);
					
				}
				else//자식 프로세스
				{
					printf("포켓몬을 치료합니다.\n\n");
					return -1;
				}
				break;
			case '2': //몬스터 볼 구매 fork
				childpid = fork();
				if (childpid == -1) {
					perror("Failed to fork");
					return 1;
				}
				if (childpid)//부모 프로세스
				{
					wait(NULL);
					Buy_Ball(&user);
					
				}
				else//자식 프로세스
				{
					printf("몬스터볼을 구매합니다.\n\n");
					return -1;
				}
				break;
                        case '3': //저장 fork
				childpid = fork();
				if (childpid == -1) {
					perror("Failed to fork");
					return 1;

				}
				if (childpid)//부모 프로세스
				{
					wait(NULL);
					save(&user);
					
				}
				else//자식 프로세스
				{
					printf("저장했습니다.\n\n");
					return -1;
				}
				break;
			default:
				printf("해당사항이 없는 선택지입니다.\n");
				break;
			}

		show_userinfo(&user);


		}
		else if (x == MAP_WIDTH - 1
			&& y == MAP_HEIGHT - 1)
		{
			//관장(fork)
			printf("날 보러왔니 Ang?\n");
                        champ(&user, monsters);
		}
		else // 포켓몬 랜덤 출현
		{
			battle = rand() % 10; // 이동하면서 10%의 확률로 몬스터 출현
			if (battle == 0)//배틀(fork)
			{
				printf("랜덤 배틀 발생 Ang!\n");
				fight(&user, monsters);
			}
		}
	}
	return 0;
}

void Initialize(user_info* user, M_Info *monsters) // 초기화 함수
{
	int b;
        
        user->ball = 100;
	user->money = 100;

	srand((unsigned int)time(NULL));

	getMonsters(user, monsters);

        for (int p = 0; p < 9; p++)
		show_status(&monsters[p]);

	printf("0~8번 중에 포켓몬을 3마리 선택하렴 :");
	for(int i = 0 ;i < 3; i++)
	{
		scanf("%d", &b);
		copy_status(&user[0], i, monsters[b]);
	}
}

void load(user_info* user, M_Info *monsters)//세이브 파일 불러오기
{
        int ifd, i, r;
        int HP1, HP2, HP3, exp1, exp2, exp3, ID1, ID2, ID3;
	char *deli = "\\";
	char buff[BUF_SIZE], *pt2;

        getMonsters(user, monsters);

	ifd = open("save_file.txt", O_RDONLY);
	if (ifd < 0) {
		printf("파일 열기 실패");
		exit(1);
                     }
        read(ifd, buff, sizeof(buff));//buff크기만큼 read해서 buff에 저장

	pt2 = strtok(buff, deli);//구분자 deli에 따라 buff의 내용을 strtok하기

        r = atoi(pt2);
        user->money = r;
        pt2 = strtok(NULL, deli);
 
        r = atoi(pt2);
        user->ball = r;
        pt2 = strtok(NULL, deli);

        r = atoi(pt2);
        user->pokemon_num = r;
        pt2 = strtok(NULL, deli);
        
        r = atoi(pt2);
        HP1 = r;
        pt2 = strtok(NULL, deli);
 
        r = atoi(pt2);
        exp1 = r;
        pt2 = strtok(NULL, deli);

        r = atoi(pt2);
        ID1 = r;
        pt2 = strtok(NULL, deli);

        load_data(&user[0], 0, monsters[ID1], HP1, exp1);//ID1에 해당하는 유저 몬스터 생성 

        r = atoi(pt2);
        HP2 = r;
        pt2 = strtok(NULL, deli);
 
        r = atoi(pt2);
        exp2 = r;
        pt2 = strtok(NULL, deli);

        r = atoi(pt2);
        ID2 = r;
        pt2 = strtok(NULL, deli);

        load_data(&user[0], 1, monsters[ID2], HP2, exp2);//ID2에 해당하는 유저 몬스터 생성 

        r = atoi(pt2);
        HP3 = r;
        pt2 = strtok(NULL, deli);
 
        r = atoi(pt2);
        exp3 = r;
        pt2 = strtok(NULL, deli);

        r = atoi(pt2);
        ID3 = r;
        pt2 = strtok(NULL, deli);
 
        load_data(&user[0], 2, monsters[ID3], HP3, exp3);//ID3에 해당하는 유저 몬스터 생성 

        close(ifd);//파일close하기
        

}

void getMonsters(user_info* user, M_Info* monsters)
{
	int n, a, b, r, p;
	
	int fd, i;
	char *deli = " ";
	char buf[BUF_SIZE], *pt1;
        
        

	fd = open("file.txt", O_RDONLY);//읽기 전용으로 file.txt파일을 오픈하기
	if (fd < 0) {
		printf("파일 열기 실패");
		exit(1);
	}

	read(fd, buf, sizeof(buf));//buf크기만큼 read해서 buf에 저장

	pt1 = strtok(buf, deli);//구분자 deli에 따라 buf의 내용을 strtok하기

	for (i = 0; i < 10; i++) {//기본 10마리의 포켓몬 정보를 u[0]~u[9]에 저장
		strcpy(monsters[i].name, pt1);
		pt1 = strtok(NULL, deli);

		r = atoi(pt1);
		monsters[i].HP = r;
		pt1 = strtok(NULL, deli);

		r = atoi(pt1);
		monsters[i].HP_MAX = r;
		pt1 = strtok(NULL, deli);

		strcpy(monsters[i].skill1, pt1);
		pt1 = strtok(NULL, deli);

		strcpy(monsters[i].skill2, pt1);
		pt1 = strtok(NULL, deli);

		strcpy(monsters[i].skill3, pt1);
		pt1 = strtok(NULL, deli);

		strcpy(monsters[i].skill4, pt1);
		pt1 = strtok(NULL, deli);

		r = atoi(pt1);
		monsters[i].attack1 = r;
		pt1 = strtok(NULL, deli);

		r = atoi(pt1);
		monsters[i].attack2 = r;
		pt1 = strtok(NULL, deli);

		r = atoi(pt1);
		monsters[i].attack3 = r;
		pt1 = strtok(NULL, deli);

		r = atoi(pt1);
		monsters[i].attack4 = r;
		pt1 = strtok(NULL, deli);

		r = atoi(pt1);
		monsters[i].skill_point1 = r;
		pt1 = strtok(NULL, deli);

		r = atoi(pt1);
		monsters[i].skill_point2 = r;
		pt1 = strtok(NULL, deli);

		r = atoi(pt1);
		monsters[i].skill_point3 = r;
		pt1 = strtok(NULL, deli);

		r = atoi(pt1);
		monsters[i].skill_point4 = r;
		pt1 = strtok(NULL, deli);

		r = atoi(pt1);
		monsters[i].exp = r;
		pt1 = strtok(NULL, deli);

		r = atoi(pt1);
		monsters[i].LV = r;
		pt1 = strtok(NULL, deli);
          
                r = atoi(pt1);
                monsters[i].ID = r;
                pt1 = strtok(NULL, deli);
	}

	close(fd);//파일close하기

	
}
	

void draw(int x, int y) //맵과 유저의 위치를 그려줌
{
	system("clear");//화면 지우기 

	for (int h = 0; h < MAP_HEIGHT; h++)
	{
		for (int w = 0; w < MAP_WIDTH; w++)
		{
			if (w == x
				&& h == y)
			{
				printf("㆚"); //유저 위치
				continue;
			}
			switch (map[h][w])
			{
			case 0://길
				printf("  ");
				break;
			case 1://벽
				printf("■ ");
				break;
			case 2://포켓몬 센터
				printf("♥ ");
				break;
			case 3://관장 
				printf("㆙");
				break;
			default:
				break;
			}
		}
		printf("\n");
	}
	printf("(%d,%d)\n", x, y);
}

int _getch(void) {
	int ch;
	struct termios old;
	struct termios new;
	tcgetattr(0, &old);
	new = old;
	new.c_lflag &= ~(ICANON | ECHO); new.c_cc[VMIN] = 1;
	new.c_cc[VTIME] = 0;
	tcsetattr(0, TCSAFLUSH, &new);
	ch = getchar();
	tcsetattr(0, TCSAFLUSH, &old);
	return ch;
}

void show_userinfo(user_info *user){
	int i;
	for(i=0; i<3; i++){
		printf("이름:%s\n",user->monsters[i].name);
		printf("최대 체력:%d\n",user->monsters[i].HP_MAX);
		printf("체력:%d\n",user->monsters[i].HP);
                printf("레벨:%d	\n",user->monsters[i].LV);
                printf("경험치:%d	\n",user->monsters[i].exp);
    
                }


	printf("유저 골드:%d\n",user->money);
	printf("유저 몬스터볼:%d\n",user->ball);

}
void champ(user_info *user, M_Info *monsters)
{
	int fight_num=0;
	int num = 0;
	int run_num;
	int win_num;
	int i;
	i = rand()%10;
         
        if(user->monsters[0].LV !=10)
                printf("아직 도전 자격이 주어지지 않았습니다.\n더 강해져서 오십시오");
        else
        {
	        printf("관장님에게 도전하시겠습니까? \n");
	        printf("3: 도전 4: 도망");
        
	        scanf("%d", &fight_num);
	        while(1)
	        {
		        if(fight_num == 3)
		        {
			        printf("리자몽과의 전투를 시작합니다.\n\n");
			        printf("어떤 포켓몬을 내보내시겠습니까? \n");
			        show_userinfo(&user[0]);
	
			        printf("첫 번째 포켓몬 : 0\n두 번째 포켓몬 : 1\n세 번째 포켓몬 : 2\n");
			        scanf("%d", &num);
			        system("clear");	
			        skill(&user[0], monsters[9], num);
			        return;
		        }

		        else if(fight_num == 4)
		        {
			        if(i < 2)
			        {
				        printf("도망치는 것을 실패했습니다!");
				        printf("리자몽과의 전투를 시작합니다. \n\n");
				        printf("어떤 포켓몬을 내보내시겠습니까? \n");
				        show_userinfo(&user[0]);
				        printf("첫 번째 포켓몬 : 0\n두 번째 포켓몬 : 1\n세 번째 포켓몬 : 2\n");
				        scanf("%d", &num);
				        system("clear");
				        skill(&user[0], monsters[9], num);
				        return;
			        }
			        else
			        {
				        printf("성공적으로 도망쳤습니다!");
				        return;
			        }
		        }

		        else
		        {	
			        break;
		        }
	        }
        }
	
}

void fight(user_info *user, M_Info *monsters)
{
	int fight_num=0;
	int num = 0;
	int run_num;
	int win_num;
	int i;
        int m_num;
	i = rand()%10;
        m_num = rand()%9;

	
	printf("야생의 %s가 나타났다! \n", monsters[m_num].name);
	printf("3: 전투 4: 도망가기:");
        
	scanf("%d", &fight_num);
	while(1)
	{
		if(fight_num == 3)
		{
			printf("야생 %s과의 전투를 시작합니다.\n\n", monsters[m_num].name);
			printf("어떤 포켓몬을 내보내시겠습니까? \n");
			show_userinfo(&user[0]);
	
			printf("첫 번째 포켓몬 : 0\n두 번째 포켓몬 : 1\n세 번째 포켓몬 : 2\n");
			scanf("%d", &num);
			system("clear");	
			skill(&user[0], monsters[m_num], num);
			return;
		}

		else if(fight_num == 4)
		{
			if(i < 2)
			{
				printf("도망치는 것을 실패했습니다!");
				printf("야생 %s과의 전투를 시작합니다.\n\n", monsters[m_num].name);
				printf("어떤 포켓몬을 내보내시겠습니까? \n");
				show_userinfo(&user[0]);
				printf("첫 번째 포켓몬 : 0\n두 번째 포켓몬 : 1\n세 번째 포켓몬 : 2\n");
				scanf("%d", &num);
				system("clear");
				skill(&user[0], monsters[m_num], num);
				return;
			}
			else
			{
				printf("성공적으로 도망쳤습니다!");
				return;
			}
		}

		else
		{	
			break;
		}
	}
	
}

void skill(user_info *user, M_Info monsters, int num)
{
	int turn = 1;
	int enemy_HP = monsters.HP; 
	int sk; //사용자 포켓몬의 스킬입력을 받는 변수
	int attack;

	while(1)
	{
		usleep(1000000);
		system("clear");
		printf("<내 포켓몬>\n%s\n 레벨 : %d\n체력 : %d\n\n", user->monsters[num].name, user->monsters[num].LV, user->monsters[num].HP);
		printf("<%s>\n\n체력 : %d\n\n", monsters.name, enemy_HP);
		if(turn == 1)
		{
			printf("어떤 기술을 사용하시겠습니까?(1, 2, 3, 4)\n\n 주의 : 잘못 입력하면 턴이 넘어갑니다.\n");
			printf("%s\n", user->monsters[num].skill1);
			printf("%s\n", user->monsters[num].skill2);
			printf("%s\n", user->monsters[num].skill3);
			printf("%s\n\n", user->monsters[num].skill4);
			scanf("%d", &sk);
			if(sk == 1)
			{
				printf("가라 %s ! %s !!", user->monsters[num].name, user->monsters[num].skill1);
				printf("\n");
				attack = user->monsters[num].attack1;
				enemy_HP -= attack;
			}
			else if(sk == 2)
			{
				printf("가라 %s ! %s !!", user->monsters[num].name, user->monsters[num].skill2);
				printf("\n");
				attack = user->monsters[num].attack2;			
				enemy_HP -= attack;
			}
			else if(sk == 3)
			{
				printf("가라 %s ! %s !!", user->monsters[num].name, user->monsters[num].skill3);
				printf("\n");
				attack = user->monsters[num].attack3;
				enemy_HP -= attack;
			}
			else if(sk == 4)
			{
				printf("가라 %s ! %s !!", user->monsters[num].name, user->monsters[num].skill4);
				printf("\n");
				attack = user->monsters[num].attack4;
				enemy_HP -= attack;
			}
			else
			{
				printf("잘못된 번호를 입력하셨습니다.");
			}
			turn = 2; //상대에게 턴이 넘어감
		}
		else if(turn == 2)
		{
			int enemy_select = rand()%2 + 1;
			if(enemy_select == 1)
			{
				printf("%s의 %s!", monsters.name, monsters.skill1);
				attack = monsters.attack1;
				user->monsters[num].HP -= attack;
			}
			else
			{
				printf("%s의 %s!", monsters.name, monsters.skill3);
				attack = monsters.attack3;
				user->monsters[num].HP -= attack;
			}
			turn = 1;
		}
		else
		{
			printf("턴 변경 오류!");
			return;
		}

		if(user->monsters[num].HP <= 0)
		{
			printf("%s가 전투 불능 상태가 되었습니다.\n\n (전투에서 패배하여 필드로 돌아갑니다.)\n",user->monsters[num].name);
                        sleep(5);
			break;
		}
		else if(enemy_HP <= 0)
		{
			printf("전투에서 승리했다!\n\n (전투에서 승리하였다!)\n");
			calculate_LV(&user[0], rand()%5 + 1);
                        sleep(5);
			break;
		}

	}
}

void calculate_LV(user_info *user, int i){//i는 넘겨받을 경험치량

	        for(int n=0; n<3; n++){
			int a = 0;
                        int b = 0;
                        int c = 0;
			user->monsters[n].exp += i;//전투 승리후 경험치를준다
			a =  user->monsters[n].LV;//원래 LV을 a에 보관          
			user->monsters[n].LV = 1+(user->monsters[n].exp/10);//LV계산              
			if(a !=  user->monsters[n].LV){//a에 보관된 기존 LV과 다르다면(=LV이 증가했다면)
				user->monsters[n].HP_MAX = 70+30*user->monsters[n].LV;//최대 체력은 30 증
				user->monsters[n].HP = user->monsters[n].HP_MAX;//현재 체력은 상승된 최대체력으로 회복시켜주기
				user->monsters[n].attack1 = 7+3*user->monsters[n].LV;//스킬1 공격력 3 증가
				user->monsters[n].attack2 = 17+3*user->monsters[n].LV;//스킬2 공격력 3 증가
				user->monsters[n].attack3 = 37+3*user->monsters[n].LV;//스킬3 공격력 3 증가
				user->monsters[n].attack4 = 47+3*user->monsters[n].LV;//스킬4 공격력 3 증가
				printf("%s의 레벨이 %d가 되었습니다!\n", user->monsters[n].name, user->monsters[n].LV);
			} 
		}

		return;
}


