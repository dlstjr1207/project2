#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#define PORT 9000       // 서버 포트
#define BUF_SIZE 65536  // 최대 버퍼 크기
// 음식점 정보 구조체 (연결리스트용)
typedef struct Restaurant {
    int id;         //고유 ID
    char sido[20];  // 시도(ex: 충남)
    char sigungu[20];   // 시군구(ex: 아산시)
    char category[50]; // 업태(카테고리)
    char menu[100]; // 대표 매뉴
    char name[100]; //상호명
    char address[200]; //주소
    char date[20];      // 지정일자
    struct Restaurant* next;    //다음 노드 포인터
} Restaurant;
// 음식점별/사용자별 별점 구조체 (연결리스트용)
typedef struct Rating {
    int rid;    // 음식점 ID
    char userid[32];    // 사용자 ID
    int score;  //별점(1~5)
    struct Rating* next;    // 다음 노드 포인터
} Rating;

Restaurant* restaurant_head = NULL;     // 음식점 리스트 헤드
Rating* rating_head = NULL;               // 별점 리스트 헤드
int restaurant_count = 0;                // 음식점 수(최대 id 관리)

// 문자열 앞뒤 공백/탭/줄바꿈 제거 함수
void trim_inplace(char* s) {
    
    int len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r' || s[len-1]==' ' || s[len-1]=='\t')) s[--len] = 0;
    
    int i = 0;
    while (s[i] == ' ' || s[i] == '\t') i++;
    if (i > 0) memmove(s, s + i, strlen(s + i) + 1);
}

// 음식점 연결리스트를 파일로 저장
void save_restaurants(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) return;
    Restaurant* p = restaurant_head;
    while (p) {
        // 모든 항목을 탭으로 구분, 업소구분은 "모범"으로 고정
        fprintf(fp, "%d\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t모범\n",
            p->id, p->sido, p->sigungu, p->category, p->menu, p->name, p->address, p->date);
        p = p->next;
    }
    fclose(fp);
}
// 음식점 리스트 파일에서 로딩 (연결리스트로)
void load_restaurants(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return;
    char buf[1024];
    int maxid = 0;
    while (fgets(buf, sizeof(buf), fp)) {
        Restaurant* r = (Restaurant*)calloc(1, sizeof(Restaurant));
        char* token = strtok(buf, "\t\n");
        int col = 0;
        while (token) {
            switch (col) {
            case 0: r->id = atoi(token); break;
            case 1: strncpy(r->sido, token, 19); break;
            case 2: strncpy(r->sigungu, token, 19); break;
            case 3: strncpy(r->category, token, 49); break;
            case 4: strncpy(r->menu, token, 99); break;
            case 5: strncpy(r->name, token, 99); break;
            case 6: strncpy(r->address, token, 199); break;
            case 7: strncpy(r->date, token, 19); break;
            }
            token = strtok(NULL, "\t\n");
            col++;
        }
        r->next = restaurant_head;
        restaurant_head = r;
        if (r->id > maxid) maxid = r->id;
    }
    restaurant_count = maxid;
    fclose(fp);
}

// 별점 연결리스트를 파일로 저장
void save_ratings(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) return;
    Rating* p = rating_head;
    while (p) {
        fprintf(fp, "%d\t%s\t%d\n", p->rid, p->userid, p->score);
        p = p->next;
    }
    fclose(fp);
}

// 별점 파일에서 로딩 (연결리스트로)
void load_ratings(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return;
    char buf[256];
    while (fgets(buf, sizeof(buf), fp)) {
        Rating* r = (Rating*)calloc(1, sizeof(Rating));
        sscanf(buf, "%d\t%s\t%d", &r->rid, r->userid, &r->score);
        r->next = rating_head;
        rating_head = r;
    }
    fclose(fp);
}

// 음식점 중복 체크 (이름+주소가 모두 동일하면 중복)
int is_duplicate(const char* name, const char* addr) {
    Restaurant* p = restaurant_head;
    while (p) {
        if (strcmp(p->name, name) == 0 && strcmp(p->address, addr) == 0)
            return 1;
        p = p->next;
    }
    return 0;
}

// ID로 음식점 노드 찾기
Restaurant* find_restaurant_by_id(int id) {
    Restaurant* p = restaurant_head;
    while (p) {
        if (p->id == id) return p;
        p = p->next;
    }
    return NULL;
}

// 음식점 삭제
void delete_restaurant(int id) {
    Restaurant* p = restaurant_head, *prev = NULL;
    while (p) {
        if (p->id == id) {
            if (prev) prev->next = p->next;
            else restaurant_head = p->next;
            free(p);
            break;
        }
        prev = p; p = p->next;
    }
    save_restaurants("restaurant.txt");
}
// 음식점 정보 수정 (지정 필드만 변경)
void modify_restaurant(Restaurant* r, const char* field, const char* val) {
    if (strcmp(field, "name")==0) strncpy(r->name, val, 99);
    else if (strcmp(field, "category")==0) strncpy(r->category, val, 49);
    else if (strcmp(field, "address")==0) strncpy(r->address, val, 199);
    else if (strcmp(field, "menu")==0) strncpy(r->menu, val, 99);
    else if (strcmp(field, "date")==0) strncpy(r->date, val, 19);
    else if (strcmp(field, "sido")==0) strncpy(r->sido, val, 19);
    else if (strcmp(field, "sigungu")==0) strncpy(r->sigungu, val, 19);
    save_restaurants("restaurant.txt");
}

// 별점 추가/갱신 (동일 user+음식점이면 갱신)
void add_rating(int rid, const char* userid, int score) {
    Rating* p = rating_head;
    while (p) {
        if (p->rid == rid && strcmp(p->userid, userid)==0) {
            p->score = score;
            save_ratings("rating.txt");
            return;
        }
        p = p->next;
    }

     // 새 별점이면 추가
    Rating* r = (Rating*)calloc(1, sizeof(Rating));
    r->rid = rid;
    strcpy(r->userid, userid);
    r->score = score;
    r->next = rating_head;
    rating_head = r;
    save_ratings("rating.txt");
}

// 음식점 평균 별점 계산 (리스트 순회)
float get_avg_rating(int rid, int* cnt) {
    int sum=0, count=0;
    Rating* p = rating_head;
    while (p) {
        if (p->rid == rid) { sum += p->score; count++; }
        p = p->next;
    }
    *cnt = count;
    if (count==0) return 0.0;
    return (float)sum/count;
}

// users.txt 인증 (로그인)
int check_user(const char* id, const char* pw) {
    FILE* fp = fopen("users.txt", "r");
    if (!fp) return 0;
    char line[64], uid[32], upw[32];
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%[^/]/%s", uid, upw);
        if (strcmp(id, uid)==0 && strcmp(pw, upw)==0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

// 업태(카테고리) 목록 클라이언트로 전송 (중복없이)
void send_category_list(SOCKET sock) {
    char list[2000] = "";
    char cats[200][50] = {0}; int n=0;
    Restaurant* p = restaurant_head;
    for (; p; p=p->next) {
        int found=0;
        for (int i=0;i<n;i++)
            if (strcmp(cats[i],p->category)==0) {found=1; break;}
        if (!found && strlen(p->category)>0) strcpy(cats[n++],p->category);
    }
    for (int i=0;i<n;i++) {
        strcat(list, cats[i]);
        if (i<n-1) strcat(list, ", ");
    }
    if (n==0) strcpy(list,"(없음)");
    send(sock, list, strlen(list), 0);
}

// 음식점 ID 오름차순 정렬용 비교 함수
int compare_id(const void* a, const void* b) {
    Restaurant* ra = *(Restaurant**)a;
    Restaurant* rb = *(Restaurant**)b;
    return ra->id - rb->id;
}

// 클라이언트별 명령 처리 (스레드용)
void process_client(SOCKET sock) {
    char buf[BUF_SIZE], login_id[32] = "";
    int login_try = 0;
    while (1) {
        memset(buf, 0, sizeof(buf));
        int len = recv(sock, buf, BUF_SIZE-1, 0);
        if (len <= 0) break;
        buf[len]=0;

        // 1. 로그인 처리
        if (strncmp(buf, "LOGIN:", 6)==0) {
            char* id = strtok(buf+6, "/");
            char* pw = strtok(NULL, ":");
            trim_inplace(pw);
            if (check_user(id, pw)) {
                strcpy(login_id, id);
                send(sock, "LOGIN_OK", 8, 0);
            } else {
                login_try++;
                send(sock, "LOGIN_FAIL", 10, 0);
                if (login_try>=3) break;
            }
        }
         // 2. 업태 목록 요청
        else if (strncmp(buf, "CATEGORY", 8)==0) {
            send_category_list(sock);
        }

        // 3. 음식점 조건 검색 (지역/업태)
        else if (strncmp(buf, "SEARCH:", 7)==0) {
            char region[32]="", cat[50]="";
            sscanf(buf+7, "%[^;];%[^;]", region, cat);
            trim_inplace(region); trim_inplace(cat);
        
            Restaurant* arr[2000]; int n=0;
            Restaurant* p = restaurant_head;
            while (p) {
                char pcat[50], pregion[64];
                strncpy(pcat, p->category, sizeof(pcat)-1); pcat[sizeof(pcat)-1]=0;
                strncpy(pregion, p->sigungu, sizeof(pregion)-1); pregion[sizeof(pregion)-1]=0;
                trim_inplace(pcat);    
                trim_inplace(pregion);
        
                int region_ok = 1, cat_ok = 1;
                if (strlen(region) > 0) {
                    region_ok = strcmp(pregion, region)==0;
                }
                if (strlen(cat) > 0) {
                    cat_ok = strcmp(pcat, cat)==0;
                    
                }
                if (region_ok && cat_ok) {
                    arr[n++] = p;
                }
                p = p->next;
            }
            qsort(arr, n, sizeof(Restaurant*), compare_id);
            char out[BUF_SIZE] = "";
            for (int i=0;i<n;i++) {
                char line[512];
                snprintf(line, sizeof(line), "%d. %s | %s | %s | %s | %s\n",
                    arr[i]->id, arr[i]->name, arr[i]->menu, arr[i]->address, arr[i]->category, arr[i]->sigungu);
                strcat(out, line);
            }
            if (n==0) strcpy(out,"검색 결과 없음.\n");
            send(sock, out, strlen(out), 0);
        }

        // 4. 음식점 상세정보 요청
        else if (strncmp(buf, "DETAIL:", 7)==0) {
            int id = atoi(buf+7);
            Restaurant* r = find_restaurant_by_id(id);
            char out[512];
            if (!r) strcpy(out, "상세정보 없음.\n");
            else {
                int cnt; float avg = get_avg_rating(r->id, &cnt);
                snprintf(out, sizeof(out),
                "상호명: %s\n주소: %s\n업태: %s\n주메뉴: %s\n지정일자: %s\n지역: %s %s\n평균 별점: %.1f (%d명)\n",
                     r->name, r->address, r->category, r->menu, r->date, r->sido, r->sigungu, avg, cnt);
            }
            send(sock, out, strlen(out), 0);
        }

        // 5. 음식점 추가 (중복검사)
        else if (strncmp(buf, "ADD:", 4)==0) {
            char name[100], category[50], address[200], menu[100], date[20], sido[20], sigungu[20];
            sscanf(buf+4, "%[^;];%[^;];%[^;];%[^;];%[^;];%[^;];%s", name, category, address, menu, date, sido, sigungu);
            trim_inplace(name); trim_inplace(category); trim_inplace(address); trim_inplace(menu); trim_inplace(date); trim_inplace(sido); trim_inplace(sigungu);
            if (is_duplicate(name, address)) {
                send(sock, "ADD_FAIL", 8, 0);
            } else {
                Restaurant* r = (Restaurant*)calloc(1, sizeof(Restaurant));
                r->id = ++restaurant_count;
                strncpy(r->name, name, 99);
                strncpy(r->category, category, 49);
                strncpy(r->address, address, 199);
                strncpy(r->menu, menu, 99);
                strncpy(r->date, date, 19);
                strncpy(r->sido, sido, 19);
                strncpy(r->sigungu, sigungu, 19);
                r->next = restaurant_head;
                restaurant_head = r;
                save_restaurants("restaurant.txt");
                send(sock, "ADD_OK", 6, 0);
            }
        }

         // 6. 음식점 삭제
        else if (strncmp(buf, "DELETE:", 7)==0) {
            int id = atoi(buf+7);
            if (find_restaurant_by_id(id)) {
                delete_restaurant(id);
                send(sock, "DELETE_OK", 9, 0);
            } else send(sock, "DELETE_FAIL", 11, 0);
        }

            // 7. 음식점 수정
        else if (strncmp(buf, "MODIFY:", 7)==0) {
            int id; char field[32], value[128];
            sscanf(buf+7, "%d:%[^:]:%[^\n]", &id, field, value);
            trim_inplace(field); trim_inplace(value);
            Restaurant* r = find_restaurant_by_id(id);
            if (!r) send(sock, "MODIFY_FAIL", 11, 0);
            else {
                modify_restaurant(r, field, value);
                send(sock, "MODIFY_OK", 9, 0);
            }
        }

          // 8. 별점 평가
        else if (strncmp(buf, "RATE:", 5)==0) {
            int id, score; char userid[32];
            sscanf(buf+5, "%d:%[^:]:%d", &id, userid, &score);
            trim_inplace(userid);
            if (score<1||score>5) send(sock, "RATE_FAIL", 9, 0);
            else {
                add_rating(id, userid, score);
                send(sock, "RATE_OK", 7, 0);
            }
        }
          // 9. 평균 별점 확인
        else if (strncmp(buf, "RANK:", 5)==0) {
            int id = atoi(buf+5);
            int cnt; float avg = get_avg_rating(id, &cnt);
            char out[128];
            snprintf(out, sizeof(out), "평균 별점: %.1f (%d명)\n", avg, cnt);
            send(sock, out, strlen(out), 0);
        }

         // 10. 클라이언트 종료
        else if (strncmp(buf, "EXIT", 4)==0) {
            break;
        }

        else send(sock, "UNKNOWN", 7, 0);
    }
    closesocket(sock);  // 소켓 닫기
}


// --- 서버 메인 함수 ---
// 소켓 초기화, 파일 로딩, 연결 대기/스레드 생성, 종료시 파일 저장
int main() {
    WSADATA wsaData;
    SOCKET listen_sock, client_sock;
    SOCKADDR_IN server_addr, client_addr;
    int addr_len = sizeof(client_addr);

    WSAStartup(MAKEWORD(2,2), &wsaData);
    listen_sock = socket(PF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    bind(listen_sock, (SOCKADDR*)&server_addr, sizeof(server_addr));
    listen(listen_sock, 10);

    load_restaurants("restaurant.txt");      // 음식점 데이터 불러오기
    load_ratings("rating.txt");             // 별점 데이터 불러오기

    printf("Server Ready (Port %d)\n", PORT);

    // 클라이언트 연결 대기 (멀티스레드)
    while (1) {
        client_sock = accept(listen_sock, (SOCKADDR*)&client_addr, &addr_len);
        if (client_sock == INVALID_SOCKET) continue;
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)process_client, (LPVOID)client_sock, 0, 0);
    }
    closesocket(listen_sock);
    WSACleanup();
    save_restaurants("restaurant.txt");     // 데이터 저장
    save_ratings("rating.txt");
    return 0;
}
