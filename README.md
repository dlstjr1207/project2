# 🍜 천안/아산 모범 음식점 정보 시스템 (C-TCP/IP)

> [cite_start]**선문대학교 컴퓨터공학과 C응용프로젝트** [cite: 1, 2]
> [cite_start]C언어 콘솔 기반의 TCP 통신을 이용한 음식점 정보 조회 및 별점 관리 시스템입니다. [cite: 5, 9]

---

## 📌 프로젝트 개요
[cite_start]본 시스템은 충청남도 공공데이터를 활용하여 사용자가 천안/아산 지역의 모범 음식점을 검색하고, 직접 별점을 남기거나 정보를 관리할 수 있는 클라이언트-서버 구조의 응용 프로그램입니다. [cite: 55, 134]

## 🛠 기술 스택
* [cite_start]**Language:** C [cite: 9, 136]
* [cite_start]**Network:** WinSock 2.2 (TCP/IP 소켓 통신) [cite: 56, 136]
* [cite_start]**Data Structure:** 구조체 기반 동적 연결리스트 (Linked List) [cite: 179, 187]
* [cite_start]**Environment:** Visual Studio 2022 [cite: 136]
* [cite_start]**Features:** Multi-threading 서버 (다중 접속 지원) [cite: 149, 160]

## ✨ 핵심 기능
1. [cite_start]**사용자 인증**: `users.txt` 기반 로그인 시스템 (최대 3회 시도 제한) [cite: 63, 65]
2. [cite_start]**음식점 조건 검색**: 지역(천안/아산), 업태, 메뉴 키워드별 필터링 [cite: 67, 139]
3. [cite_start]**정보 관리(CRUD)**: 음식점 정보의 추가, 수정, 삭제 기능 [cite: 142]
4. [cite_start]**사용자 참여**: 실시간 별점 평가 및 평균 별점 산출 기능 [cite: 141, 291]
5. [cite_start]**데이터 동기화**: 프로그램 종료 시 모든 데이터를 `txt` 파일로 백업 [cite: 50, 60]

## 📂 파일 구조
* [cite_start]`server.c`: 클라이언트의 요청을 처리하고 데이터를 관리하는 멀티스레드 서버 [cite: 160]
* [cite_start]`client.c`: 사용자 인터페이스(UI)를 제공하는 클라이언트 프로그램 [cite: 151]
* [cite_start]`restaurant.txt`: 공공데이터 기반 음식점 정보 저장 파일 [cite: 171]
* [cite_start]`users.txt` / `rating.txt`: 사용자 계정 및 별점 데이터 파일 [cite: 138, 308]
* [cite_start]`docs/`: 개발 계획서 및 요구사항 분석서 PDF 문서 [cite: 3, 363]

---
## 👨‍💻 작성자
* [cite_start]**성명:** 주인석 (선문대학교 컴퓨터공학부 3학년) [cite: 1, 20, 21]
* [cite_start]**학번:** 2022243027 [cite: 22]
