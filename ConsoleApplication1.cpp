// ConsoleApplication1.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <mysql.h>
#include <winsock2.h>
#include <string.h>
#include <Windows.h>
#pragma comment(lib, "libmariadb.lib")

using namespace std;

#define MYSQLUSER "root"
#define MYSQLPASS "spacezone"
#define MYSQLIP "localhost"

void Connect(const char sqlip[], MYSQL* conn)
{
	if (conn == NULL) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		Sleep(1000);
		exit(1);
	}
	if (!(mysql_real_connect(conn, MYSQLIP, MYSQLUSER, MYSQLPASS, "test", 3307, NULL, 0) == NULL)) //mysql서버로 연결 성공하면 0 아니면 다른수 반환
	{
		mysql_set_character_set(conn, "euckr");				//MySQL 문자를 지정한다. 만약 안하면 한글이 짤림.
	}
	else {
		fprintf(stderr, "연결 오류 : %s\n", mysql_error(conn));
		exit(1);
	}
}

void Main_Menu() {
	cout << "---------------------\n"
         << "1. Load Data\n"
	     << "2. Insert Data\n"
	     << "3. Update Data\n"
	     << "4. Delete Data\n"
		 << "5. Access Blog\n"
	     << "6. Quit\n"
	     << "---------------------\n";
}

void Load(MYSQL* conn) {

	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row; // = char **sql_row; 2차원 배열 = 1차원 문자열 배열
	mysql_query(conn, "select * from test.info");
	sql_result = mysql_store_result(conn);
	while (sql_row = mysql_fetch_row(sql_result)) { //값이 없을때까지 변환함
		cout << sql_row[0] << " " <<  sql_row[1] << " " << sql_row[2] << " " << sql_row[3] << endl;
	}
	mysql_free_result(sql_result);
}

void Insert(MYSQL* conn) {

	int i = 0;
	char query[255];
	string data[3];

	cout << "type your name, tel_number and blog url. Allowed blank tel_number and blog url \n"      //allowed blank 라 써 놓긴 했지만 공백을 어떻게 입력을 받아야 할지 몰라서
		 << "name : ";                                                                               //완벽하게 구현은 하지 못했다. (일일이 다 채워 넣어야 한다...)
	cin >> data[0];                                                                                  //getline, cin.getline, cin, scanf 이것 저것 다 넣어보았지만 그냥 스페이스, 혹은 엔터는 인식을 못한다
	cin.clear();

	cout << "tel_number : ";
	cin >> data[1];
	cin.clear();

	cout << "blog url : ";
	cin >> data[2];
	cin.clear();

	sprintf_s(query, "INSERT INTO `test`.`info` (`User_ID`, `Tel_num`, `blog`) VALUES ('%s', '%s', '%s');", data[0].c_str(), data[1].c_str(), data[2].c_str());
    mysql_query(conn, query);
}

void Delete_D(MYSQL* conn) {

	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row; 
	mysql_query(conn, "select * from test.info");                               //Load함수를 불러와야하지만 그렇게 되면 result데이터가 날아가기에 row 갯수를 추출해낼 수 없다,,,
	sql_result = mysql_store_result(conn);
	while (sql_row = mysql_fetch_row(sql_result)) { 
		cout << sql_row[0] << " " << sql_row[1] << " " << sql_row[2] << " " << sql_row[3] << endl;
	}

	int row_count = mysql_num_rows(sql_result);

	mysql_free_result(sql_result);
	char query[255];
	cout << "Input the Number of row what you want to Delete" << endl;           //blog_access와 같이 DB에서 넘어온 Row Number와 실제 Row Number이 다를때가 있다
	int i;                                                                       //이는 DB에서 Row Number를 auto increase로 설정해 놓아서 그렇다
	cin >> i;                                                                    //이를 해결하기 위해서는 delete문이 끝난 다음 row number를 update쿼리 문을 사용해서 데이터를 정렬해주어야 한다...
	cin.clear();

	if (i > row_count) {
		cout << "Wrong Number" << endl;
	}
	else {
		sprintf_s(query, "DELETE FROM `test`.`info` WHERE Num = %d", i);
		mysql_query(conn, query);
	}
}

void blog_access(MYSQL* conn) {                                                  //어떤 기능을 넣을까 생각하다가 기본 DB 구성 테이블 중에 url이 들어가 있는 걸
	                                                                             //보고 c++에서 url을 실행시키면 어떨까 하고 구현 하였다.
	MYSQL_RES* sql_result;
	int i = 0, k = 0;
	mysql_query(conn, "select `Num`, `User_ID`, `Blog` from test.info;");
	sql_result = mysql_store_result(conn);

	int row_count = mysql_num_rows(sql_result);

	char*** name = (char***)malloc(16 * row_count);                              //malloc을 쓸때 크기를 지정해주는 항목에 char나 char*를 넣고 돌리면 자꾸 heap currption오류가 뜬다....
	                                                                             //그래서 그냥 생으로 16바이트를 지정해 주었다.... 어디가 문제일까...
	while (name[i++] = mysql_fetch_row(sql_result));                             //현재도 버퍼 오버런 현상이 발생할 수 있다고 뜬다.... malloc이 동적할당이 아니였는가?
	mysql_free_result(sql_result);                                               //또 저기 조건문에 포인터 형식을 넣으면 NULL 포인터를 역참조 하고 있다고 뜨는데 이것은 또 무엇인가... 돌아가기는 하는데..
	
	for (i = 0; i < row_count; i++)
	{
		for (int k = 0; k < 3; k++)
			cout << name[i][k] << " ";
		cout << endl;
	}

	cout << "select Number what you want to access" << endl;
	cin >> i;
	cin.clear();

	i = i-1;

	if (i > row_count) {
		cout << "Wrong Number" << endl;
	}else {
		if (!strcmp(name[i][2], "(NULL)")) {
			cout << "No URL Here\n";
		}
		else {
			ShellExecute(NULL, "open", name[i][2], NULL, NULL, SW_HIDE);         //url 실행시키는 함수
		}
	}

	delete(name);                                                              //할당 해제 해주기 위해 free를 써보았지만 모호하다면서 빠꾸를 먹었다. 그냥 함수를 따로 만들까 생각했는데 이 함수에서만
}                                                                              //사용하기엔 뭔가 좀 그래서 통째로 집어넣었다만.... 그냥 함수로 만들걸 그랬나...

int main()
{
	MYSQL* conn = mysql_init(NULL);
	Connect(MYSQLIP, conn);
	int a;
	while (true) {
		//system("cls");
		Main_Menu();
		cin >> a;
		cin.clear();
		switch (a)
		{
		case 1: //system("cls"); 
			Load(conn);
			//system("pause");
			continue;
		case 2: //system("cls"); 
			Insert(conn);
			//system("pause");
			continue;
//		case 3: Update(conn);                                                //update는 미구현
//			continue;
		case 4: //system("cls"); 
			Delete_D(conn);
			//system("pause");
			continue;
		case 5: //system("cls"); 
			blog_access(conn);
			//system("pause");
			continue;
		case 6: cout << "Good bye";
			break;
		default: cout << "Wrong Input" << endl;
			continue;
		}
		break;
	}
	mysql_close(conn);
	return 0;
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
