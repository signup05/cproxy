# 아키텍처 메모

## 목표

`cproxy`는 단순한 socket 예제를 유지보수 가능한 C 프로젝트 구조로 정리한 HTTP forward proxy입니다. 이 문서는 코드가 어떤 책임으로 나뉘어 있고, 요청 하나가 어떤 흐름으로 처리되는지 설명합니다.

## 전체 실행 흐름

1. `src/main.c`
   - 실행 인자로 받은 포트를 확인합니다.
   - 인자가 없으면 기본 포트 `8080`을 사용합니다.
   - `SIGPIPE`를 무시하도록 설정해 끊어진 연결 때문에 프로세스가 종료되지 않게 합니다.
   - `run_proxy_server()`를 호출해 프록시 서버를 시작합니다.

2. `src/proxy.c`
   - listening socket을 생성하고 지정된 포트에 bind합니다.
   - 클라이언트 연결을 `accept()`로 받습니다.
   - 연결마다 detached worker thread를 생성합니다.
   - 요청 수신, HTTP 파싱, upstream 연결, 응답 relay 과정을 조율합니다.

3. `src/http.c`
   - 클라이언트가 보낸 HTTP 요청 헤더를 읽습니다.
   - method, target, version, host, port, path를 파싱합니다.
   - forward proxy 요청의 absolute-form target을 origin-form path로 변환합니다.
   - upstream 서버에 TCP 연결을 생성합니다.
   - upstream 응답을 클라이언트에게 끝까지 전달합니다.

## 주요 설계 선택

### Thread-per-connection 구조

클라이언트 연결 하나마다 worker thread 하나를 생성합니다. 이 방식은 대규모 트래픽 처리에는 한계가 있지만, `pthread`를 이용한 동시성 처리 흐름을 명확하게 보여주기 좋습니다.

포트폴리오 관점에서는 다음 내용을 설명하기 쉽습니다.

- accept loop와 worker thread의 역할 분리
- heap에 저장한 context를 thread로 전달하는 방식
- detached thread를 사용했을 때의 장단점
- 이후 `select`, `poll`, `epoll` 기반 구조로 확장할 수 있는 지점

### 요청 정규화

forward proxy는 클라이언트로부터 다음과 같은 absolute-form 요청을 받을 수 있습니다.

```http
GET http://example.com/index.html HTTP/1.1
```

하지만 일반 origin server는 보통 다음과 같은 origin-form 요청을 기대합니다.

```http
GET /index.html HTTP/1.1
```

`cproxy`는 요청을 upstream에 보내기 전에 target을 origin-form으로 재구성합니다. 또한 proxy 전용 헤더인 `Proxy-Connection`과 기존 `Connection` 헤더를 제거하고, upstream 연결을 명확히 닫기 위해 `Connection: close`를 추가합니다.

### 오류 처리

요청 파싱 실패, upstream 연결 실패, 지원하지 않는 `CONNECT` 요청처럼 흔히 발생할 수 있는 실패 상황에 대해 간단한 HTTP 오류 응답을 반환합니다.

- 잘못된 요청: `400 Bad Request`
- upstream 연결 또는 전달 실패: `502 Bad Gateway`
- HTTPS tunneling 요청: `501 Not Implemented`

이 처리는 테스트를 쉽게 만들고, 코드 리뷰나 면접에서 실패 경로를 설명하기 좋게 합니다.

## 현재 한계

현재 구현은 학습과 포트폴리오 설명을 위한 경량 범위에 맞춰져 있습니다.

- HTTPS `CONNECT` tunneling을 지원하지 않습니다.
- 큰 request body streaming을 완전하게 처리하지 않습니다.
- socket timeout과 graceful shutdown 처리가 없습니다.
- connection pooling이 없습니다.
- `epoll` 기반 event loop가 아니라 thread-per-connection 구조입니다.
- 운영용 공개 프록시에 필요한 인증, 접근 제어, rate limit 기능이 없습니다.

이 한계들은 결함이라기보다 다음 확장 방향을 설명하기 위한 명확한 범위 정의입니다.
