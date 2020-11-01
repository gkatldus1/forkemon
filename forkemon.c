#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "pokemon_info.h"
 

void show_status(M_Info *p){//p의 구조체 정보를 출력
	printf("이름:%s\n", p->name);
        printf("HP:%d\n", p->HP);
	printf("스킬1:%s 공격력:%d\n", p->skill1, p->attack1);
        printf("스킬2:%s 공격력:%d\n", p->skill2, p->attack2);
        printf("스킬3:%s 공격력:%d\n", p->skill3, p->attack3);
        printf("스킬4:%s 공격력:%d\n", p->skill4, p->attack4);
        printf("LV:%d exp:%d\n", p->LV, p->exp);

	return;
}

void copy_status(user_info *user, int i, M_Info monster){//b의 구조체 정보를 a의 구조체에 저장
        strcpy(user->monsters[i].name, monster.name);
        user->monsters[i].HP = monster.HP_MAX;
        user->monsters[i].HP_MAX = monster.HP_MAX;
        strcpy(user->monsters[i].skill1,monster.skill1);
        strcpy(user->monsters[i].skill2,monster.skill2);
        strcpy(user->monsters[i].skill3,monster.skill3);
        strcpy(user->monsters[i].skill4,monster.skill4);
        user->monsters[i].attack1 = monster.attack1;
        user->monsters[i].attack2 = monster.attack2;
        user->monsters[i].attack3 = monster.attack3;
        user->monsters[i].attack4 = monster.attack4;
        user->monsters[i].skill_point1 = monster.skill_point1;
        user->monsters[i].skill_point2 = monster.skill_point2;
        user->monsters[i].skill_point3 = monster.skill_point3;
        user->monsters[i].skill_point4 = monster.skill_point4;
        user->monsters[i].exp = 0;//새롭게 생성되었으므로 경험치는 0
        user->monsters[i].LV = 1;//새롭게 생성되었으므로 LV은 1
        user->monsters[i].ID = monster.ID;
} 

void load_data(user_info *user, int i, M_Info monster, int a, int b){//세이브 파일에 있는 유저 몬스터 정보(HP, exp, ID)를 토대로 유저 몬스터 구조체 초기화
        strcpy(user->monsters[i].name, monster.name);//ID에 따라 해당 정보 복사
        user->monsters[i].HP = a;//세이브 파일에 있던 체력정보 a를 현재체력에 대입
        user->monsters[i].HP_MAX = monster.HP_MAX;
        strcpy(user->monsters[i].skill1,monster.skill1);
        strcpy(user->monsters[i].skill2,monster.skill2);
        strcpy(user->monsters[i].skill3,monster.skill3);
        strcpy(user->monsters[i].skill4,monster.skill4);
        user->monsters[i].attack1 = monster.attack1;
        user->monsters[i].attack2 = monster.attack2;
        user->monsters[i].attack3 = monster.attack3;
        user->monsters[i].attack4 = monster.attack4;
        user->monsters[i].skill_point1 = monster.skill_point1;
        user->monsters[i].skill_point2 = monster.skill_point2;
        user->monsters[i].skill_point3 = monster.skill_point3;
        user->monsters[i].skill_point4 = monster.skill_point4;
        user->monsters[i].exp = 0;
        user->monsters[i].ID = monster.ID;
        int c = 0;
        user->monsters[i].exp += b;//세이브 파일의 경험치정보 b를 exp에 대입
        c = user->monsters[i].LV;//원래 LV을 a에 보관
        user->monsters[i].LV = ((user->monsters[i].exp)/10)+1;//LV계산
        if(c != user->monsters[i].LV){//a에 보관된 기존 LV과 다르다면
                user->monsters[i].HP_MAX += 30*(user->monsters[i].LV)-30;
                user->monsters[i].attack1 += 3*(user->monsters[i].LV)-3;
                user->monsters[i].attack2 += 3*(user->monsters[i].LV)-3;
                user->monsters[i].attack3 += 3*(user->monsters[i].LV)-3;
                user->monsters[i].attack4 += 3*(user->monsters[i].LV)-3;
                }
} 

void save(user_info* user)//세이브 파일에 저장
{
        int ofd, i;
        char *n = "\\";//구분자로 쓰일 '\'문자
        char s1[10];//정수형 자료를 문자열로 변환 후에 담아둘 문자열 
        char s2[10];
        char s3[10];
        
        sprintf(s1, "%d", user->money);//정수형 자료 money 문자열로 변환
        sprintf(s2, "%d", user->ball);//정수형 자료 ball 문자열로 변환
        sprintf(s3, "%d", user->pokemon_num);//정수형 자료 pokemon_num 문자열로

	ofd=open("save_file.txt", O_RDWR|O_TRUNC|O_CREAT, 0644);//save_file열기
         
        write(ofd, &s1, strlen(s1));//s1의 열 길이만큼 s1내용을 파일에 쓰기
        write(ofd, n, strlen(n));//구분자로 쓰일 '\' 문자 넣어주기
	write(ofd, &s2, strlen(s2));
        write(ofd, n, strlen(n));
        write(ofd, &s3, strlen(s3));
        write(ofd, n, strlen(n));

        for (i = 0; i < 3; i++) {
                char HP[10];//정수형 자료를 문자열로 변환 후에 담아둘 문자열 
                char ID[10];
                char exp[10];
                sprintf(HP, "%d", user->monsters[i].HP);
                write(ofd, &HP, strlen(HP));
                write(ofd, n, strlen(n));
             
                sprintf(exp, "%d", user->monsters[i].exp);
                write(ofd, &exp, strlen(exp));
                write(ofd, n, strlen(n));
        
                sprintf(ID, "%d", user->monsters[i].ID);
                write(ofd, &ID, strlen(ID));
                write(ofd, n, strlen(n));
                }

	close(ofd);
}

void cure(user_info* user, int pokemon_num)//유저 몬스터 치료
{
        int i;
        for (i = 0; i < pokemon_num; i++)
        {
                user->monsters[i].HP = user->monsters[i].HP_MAX;
                user->monsters[i].skill_point1 = 20;
                user->monsters[i].skill_point2 = 20;
                user->monsters[i].skill_point3 = 20;
                user->monsters[i].skill_point4 = 20;
        }
}

void Buy_Ball(user_info* user)//몬스터볼 구매
{
	int buy_num;
	printf("몇 개의 몬스터 볼을 구매하실 건가요 ? \n");
	scanf("%d", &buy_num);
	user->ball += buy_num;
	user->money -= buy_num * 5;
        
}
