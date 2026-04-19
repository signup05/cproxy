# 포트폴리오 요약

## 프로젝트명

C 기반 Linux HTTP Forward Proxy 서버

## 요약

`cproxy`는 POSIX socket과 `pthread`를 사용해 구현한 멀티스레드 HTTP forward proxy입니다. 단일 파일 형태의 socket 예제에서 출발해, 요청 파싱과 upstream relay 흐름을 모듈별로 분리한 작은 시스템 프로그래밍 프로젝트입니다.

이 프로젝트는 실제 운영용 프록시라기보다 네트워크 서버의 기본 구조, HTTP 요청 처리 방식, C 프로젝트 구성 능력을 보여주기 위한 포트폴리오 예제에 가깝습니다.

## 보여줄 수 있는 역량

- TCP socket server의 기본 생명주기 이해
- `getaddrinfo`, `socket`, `bind`, `listen`, `accept`, `connect` 사용
- `pthread`를 이용한 동시 클라이언트 처리
- HTTP request line과 header 파싱
- forward proxy 요청을 upstream 서버가 이해할 수 있는 형태로 변환
- 부분 송신과 relay loop를 고려한 socket I/O 처리
- 오류 상황에 대한 HTTP 응답 반환
- header와 source 파일을 분리한 C 프로젝트 구조화
- 프로젝트 범위와 한계를 문서로 설명하는 능력

## 기존 단순 예제 대비 개선점

- 서버 시작부, proxy 제어 흐름, HTTP 처리 로직을 파일별로 분리했습니다.
- `Makefile`을 통해 빌드 과정을 단순화했습니다.
- 한 번만 응답을 읽는 방식이 아니라 upstream 응답을 반복해서 relay합니다.
- request line과 `Host` 헤더를 파싱해 목적지 host, port, path를 명확히 추출합니다.
- `Proxy-Connection` 같은 proxy 전용 헤더를 정리합니다.
- 파싱 실패나 upstream 실패에 대해 최소한의 HTTP 오류 응답을 반환합니다.
- README와 설계 문서를 통해 현재 범위와 확장 방향을 명확히 남겼습니다.

## 면접에서 설명하기 좋은 질문

- 왜 처음 구현은 thread-per-connection 구조로 선택했는가?
- forward proxy의 absolute-form 요청과 origin server의 origin-form 요청은 어떻게 다른가?
- `CONNECT`를 지원하려면 현재 구조에서 어느 지점에 기능을 추가해야 하는가?
- 큰 `POST` 요청이나 chunked transfer를 처리하려면 무엇이 더 필요한가?
- 운영용 프록시로 발전시키려면 timeout, logging, access control을 어떻게 추가할 것인가?
- thread 기반 구조를 `select`, `poll`, `epoll` 기반 구조로 바꾸면 어떤 장단점이 있는가?

## 향후 확장 방향

- HTTPS `CONNECT` tunneling 지원
- request body streaming과 chunked transfer 처리
- socket timeout과 graceful shutdown 추가
- structured logging과 metrics 수집
- mock upstream server 기반 integration test
- reverse proxy 또는 경량 API gateway 방향으로 확장
