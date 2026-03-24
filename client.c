#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#define SERVER_IP "127.0.0.1" // 서버 IP (로컬 테스트)
#define SERVER_PORT 9000 // 서버 포트
#define BUF_SIZE 65536 // 버퍼 크기기

void menu();    // 메인 메뉴 출력
void login(SOCKET sock, char* userid);  // 로그인 처리
void search(SOCKET sock);    // 음식점 조건 검색
void detail(SOCKET sock);    // 상세정보 보기
void add_restaurant(SOCKET sock); // 음식점 추가
void delete_restaurant(SOCKET sock); // 음식점 삭제
void modify_restaurant(SOCKET sock);// 음식점 정보 수정
void rate(SOCKET sock);  // 음식점 별점 평가
void rank(SOCKET sock); // 음식점 평균 별점 확인

int main() {
    WSADATA wsaData;
    SOCKET sock;
    SOCKADDR_IN server_addr;
    char userid[32] = {0};       // 로그인한 사용자 ID
    int menu_num, run = 1;

      // WinSock 초기화
    WSAStartup(MAKEWORD(2,2), &wsaData);

    // TCP 소켓 생성
    sock = socket(PF_INET, SOCK_STREAM, 0);

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

      // 서버 연결 시도
    if (connect(sock, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("서버 연결 실패\n");
        exit(1);
    }

    printf("충남 천안/아산 모범음식점 정보 시스템 (Client)\n");

      // 로그인 처리 (성공해야 이후 메뉴 사용 가능)
    login(sock, userid);

     // 메인 메뉴 반복
    while (run) {
        menu();
        printf("메뉴 선택 > ");
        scanf("%d", &menu_num); getchar();  // 버퍼 정리
        switch (menu_num) {
            case 1: search(sock); break;    // 조건 검색
            case 2: detail(sock); break;     // 상세정보
            case 3: add_restaurant(sock); break; // 추가
            case 4: delete_restaurant(sock); break; // 삭제
            case 5: modify_restaurant(sock); break;  // 수정
            case 6: rate(sock); break;   // 별점 평가
            case 7: rank(sock); break;  // 평균 별점
            case 8: // 종료
                send(sock, "EXIT", 4, 0);
                printf("프로그램을 종료합니다.\n");
                run = 0;
                break;
            default: printf("잘못된 입력입니다.\n");
        }

    }

     // 소켓/라이브러리 정리
    closesocket(sock);
    WSACleanup();
    return 0;
}

// --- 메뉴 출력 함수 ---
void menu() {
    printf("\n--- 메뉴 ---\n");
    printf("1. 음식점 조건 검색\n");
    printf("2. 음식점 상세정보 보기\n");
    printf("3. 음식점 추가\n");
    printf("4. 음식점 삭제\n");
    printf("5. 음식점 정보 수정\n");
    printf("6. 음식점 별점 평가\n");
    printf("7. 음식점 평균 별점 확인\n");
    printf("8. 종료\n");
}

// --- 로그인 함수 ---
// 3회 실패 시 프로그램 종료
void login(SOCKET sock, char* userid) {
    char id[32], pw[32], buf[BUF_SIZE];
    int try_cnt = 0, len;
    while (1) {
        printf("[로그인]\n아이디: ");
        fgets(id, sizeof(id), stdin); id[strcspn(id, "\n")] = 0;
        printf("비밀번호: ");
        fgets(pw, sizeof(pw), stdin); pw[strcspn(pw, "\n")] = 0;
        sprintf(buf, "LOGIN:%s/%s", id, pw);
        send(sock, buf, strlen(buf), 0);
        len = recv(sock, buf, BUF_SIZE-1, 0); buf[len]=0;
        if (strncmp(buf, "LOGIN_OK", 8)==0) {
            printf("로그인 성공!\n");
            strcpy(userid, id);
            break;
        } else {
            printf("로그인 실패. 다시 입력해주세요.\n");
            try_cnt++;
            if (try_cnt >= 3) {
                printf("3회 실패. 프로그램을 종료합니다.\n");
                closesocket(sock);
                WSACleanup();
                exit(0);
            }
        }
    }
}

// --- 음식점 조건 검색 ---
// 지역(엔터시 전체), 업태(엔터시 전체) 입력 후 서버에 요청, 결과 출력
void search(SOCKET sock) {
    char region[32]="", cat[50]="", buf[BUF_SIZE];
    printf("지역 선택 (천안시, 아산시, 엔터시 전체): "); fgets(region, sizeof(region), stdin); region[strcspn(region,"\n")]=0;
    printf("[업태(카테고리) 목록]\n");
    send(sock, "CATEGORY", 8, 0);    // 서버에 업태 목록 요청
    int len = recv(sock, buf, BUF_SIZE-1, 0); buf[len]=0;
    printf("%s\n", buf);
    printf("업태 선택 (위 목록/엔터시 전체): "); fgets(cat, sizeof(cat), stdin); cat[strcspn(cat,"\n")]=0;

    sprintf(buf, "SEARCH:%s;%s", region, cat);  // 서버 프로토콜 맞춰 전송
    send(sock, buf, strlen(buf), 0);
    len = recv(sock, buf, BUF_SIZE-1, 0); buf[len]=0;
    printf("\n[검색 결과]\n%s\n", buf);
}

// --- 음식점 상세정보 요청 ---
void detail(SOCKET sock) {
    int id;
    char buf[BUF_SIZE];
    printf("상세정보를 볼 음식점 번호(ID): ");
    scanf("%d", &id); getchar();
    sprintf(buf, "DETAIL:%d", id);
    send(sock, buf, strlen(buf), 0);
    int len = recv(sock, buf, BUF_SIZE-1, 0); buf[len]=0;
    printf("[상세 정보]\n%s\n", buf);
}

// --- 음식점 추가 ---
void add_restaurant(SOCKET sock) {
    char name[100], category[50], address[200], menu[100], date[20], sido[20], sigungu[20], buf[BUF_SIZE];
    printf("상호명: "); fgets(name, sizeof(name), stdin); name[strcspn(name,"\n")]=0;
    printf("업태: "); fgets(category, sizeof(category), stdin); category[strcspn(category,"\n")]=0;
    printf("주소: "); fgets(address, sizeof(address), stdin); address[strcspn(address,"\n")]=0;
    printf("주메뉴: "); fgets(menu, sizeof(menu), stdin); menu[strcspn(menu,"\n")]=0;
    printf("지정일자: "); fgets(date, sizeof(date), stdin); date[strcspn(date,"\n")]=0;
    printf("시도(충남): "); fgets(sido, sizeof(sido), stdin); sido[strcspn(sido,"\n")]=0;
    printf("시군구(천안시/아산시 등): "); fgets(sigungu, sizeof(sigungu), stdin); sigungu[strcspn(sigungu,"\n")]=0;
    sprintf(buf, "ADD:%s;%s;%s;%s;%s;%s;%s", name, category, address, menu, date, sido, sigungu);
    send(sock, buf, strlen(buf), 0);
    int len = recv(sock, buf, BUF_SIZE-1, 0); buf[len]=0;
    if (strncmp(buf, "ADD_OK", 6)==0)
        printf("추가 성공!\n");
    else
        printf("추가 실패(중복): 이미 동일한 이름+주소가 존재합니다.\n");
}

// --- 음식점 삭제 ---
void delete_restaurant(SOCKET sock) {
    int id;
    char buf[BUF_SIZE];
    printf("삭제할 음식점 번호(ID): ");
    scanf("%d", &id); getchar();
    sprintf(buf, "DELETE:%d", id);
    send(sock, buf, strlen(buf), 0);
    int len = recv(sock, buf, BUF_SIZE-1, 0); buf[len]=0;
    if (strncmp(buf, "DELETE_OK", 9)==0)
        printf("삭제 성공!\n");
    else
        printf("삭제 실패: ID 불일치.\n");
}

// --- 음식점 정보 수정 ---
void modify_restaurant(SOCKET sock) {
    int id;
    char field[32], value[128], buf[BUF_SIZE];
    printf("수정할 음식점 번호(ID): ");
    scanf("%d", &id); getchar();
    printf("수정할 항목명(name/category/address/menu/date/sido/sigungu): ");
    fgets(field, sizeof(field), stdin); field[strcspn(field,"\n")]=0;
    printf("새 값: ");
    fgets(value, sizeof(value), stdin); value[strcspn(value,"\n")]=0;
    sprintf(buf, "MODIFY:%d:%s:%s", id, field, value);
    send(sock, buf, strlen(buf), 0);
    int len = recv(sock, buf, BUF_SIZE-1, 0); buf[len]=0;
    if (strncmp(buf, "MODIFY_OK", 9)==0)
        printf("수정 성공!\n");
    else
        printf("수정 실패(ID 불일치).\n");
}

// --- 음식점 별점 평가 ---
void rate(SOCKET sock) {
    int id, score;
    char userid[32], buf[BUF_SIZE];
    printf("별점 평가할 음식점 번호(ID): ");
    scanf("%d", &id); getchar();
    printf("본인 아이디: ");
    fgets(userid, sizeof(userid), stdin); userid[strcspn(userid,"\n")]=0;
    printf("별점(1~5): ");
    scanf("%d", &score); getchar();
    sprintf(buf, "RATE:%d:%s:%d", id, userid, score);
    send(sock, buf, strlen(buf), 0);
    int len = recv(sock, buf, BUF_SIZE-1, 0); buf[len]=0;
    if (strncmp(buf, "RATE_OK", 7)==0)
        printf("별점 등록/갱신 성공!\n");
    else
        printf("별점 등록 실패(1~5점만 입력).\n");
}

// --- 음식점 평균 별점 확인 ---
void rank(SOCKET sock) {
    int id;
    char buf[BUF_SIZE];
    printf("평균 별점 확인할 음식점 번호(ID): ");
    scanf("%d", &id); getchar();
    sprintf(buf, "RANK:%d", id);
    send(sock, buf, strlen(buf), 0);
    int len = recv(sock, buf, BUF_SIZE-1, 0); buf[len]=0;
    printf("[평균 별점]\n%s\n", buf);
}
