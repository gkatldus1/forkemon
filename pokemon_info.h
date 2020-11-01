#ifndef POKEMON_DATA_H
#define POKEMON_DATA_H
#define MONSTER_SIZE 3

typedef struct monster_Info {
	char name[20];//이름
	int HP; //현재 체력
	int HP_MAX;//최대 체력
	char skill1[20];//1스킬 이름
	char skill2[20];
	char skill3[20];
	char skill4[20];
	int attack1;//1스킬 공격력
	int attack2;
	int attack3;
	int attack4;
	int skill_point1;//1스킬 스킬포인트
	int skill_point2;
	int skill_point3;
	int skill_point4;
	int exp;//경험치
	int LV;//레벨
        int ID;//식별번호
}M_Info;

typedef struct user_info_ {
	M_Info monsters[MONSTER_SIZE];
	int money;
	int ball;
	int pokemon_num;
}user_info;



void show_status(M_Info *p);//몬스터 정보 표시
void copy_status(user_info *user, int i, M_Info monster);//스탯 복사
void save(user_info* user);//유저 구조체 정보 세이브파일에 저장하기
void load_data(user_info *user, int i, M_Info monster, int a, int b);//세이브파일 정보대로 유저 포켓몬 불러오기
void cure(user_info* user, int pokemon_num);//유저 몬스터 치료	
void Buy_Ball(user_info* user);//몬스터볼 구매					















#endif
