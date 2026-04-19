# cproxy

`cproxy`는 Linux 환경에서 동작하는 C 기반 멀티스레드 HTTP forward proxy 예제입니다. POSIX socket, `pthread`, HTTP 요청 파싱, upstream relay 흐름을 작은 코드베이스 안에서 확인할 수 있도록 구성했습니다.

이 프로젝트는 이후 `mini-api-gateway-c`로 이어지는 선행 프로젝트입니다. `cproxy`에서는 "클라이언트 요청을 받아 upstream 서버로 전달하고 응답을 되돌려주는 기본 프록시 흐름"에 집중하고, `mini-api-gateway-c`에서는 이 기반 위에 routing, policy, logging, metrics 같은 API gateway 성격의 책임을 단계적으로 확장하는 방향을 목표로 합니다.

## 주요 특징

- 기본 포트 `8080` 또는 실행 인자로 받은 포트에서 프록시 서버 실행
- 클라이언트 연결마다 worker thread를 생성하는 thread-per-connection 구조
- HTTP request line과 `Host` 헤더 파싱
- forward proxy의 absolute-form 요청을 upstream 서버용 origin-form 요청으로 변환
- `Proxy-Connection`, `Connection` 헤더 정리 후 upstream에 요청 전달
- upstream 응답을 클라이언트로 끝까지 relay
- 잘못된 요청이나 upstream 연결 실패에 대해 간단한 HTTP 오류 응답 반환

## 현재 범위와 한계

이 프로젝트는 학습과 포트폴리오 설명에 초점을 둔 경량 구현입니다.

- 일반 HTTP forwarding만 지원합니다.
- HTTPS `CONNECT` tunneling은 지원하지 않습니다.
- 큰 `POST` 요청 body streaming은 아직 완전하게 처리하지 않습니다.
- 캐싱, 접근 제어, 영속 로그, timeout 정책은 포함하지 않습니다.
- 운영용 공개 프록시가 아니라 네트워크 프로그래밍 학습용 예제입니다.

이 제한은 의도적인 범위 조절입니다. `cproxy`는 proxy의 최소 동작 원리를 분리해서 이해하기 위한 기반이고, 더 많은 정책과 운영 기능은 다음 프로젝트인 `mini-api-gateway-c`에서 다루는 것이 자연스럽습니다.

## 프로젝트 구조

```text
cproxy/
├── Makefile
├── README.md
├── docs/
│   └── architecture.md
├── include/
│   ├── http.h
│   └── proxy.h
└── src/
    ├── http.c
    ├── main.c
    └── proxy.c
```

## 빌드

필요 도구:

- `gcc`
- `make`
- Linux 또는 POSIX socket API를 사용할 수 있는 환경

빌드 명령:

```bash
make
```

정리:

```bash
make clean
```

## 실행

기본 포트 `8080`으로 실행:

```bash
./cproxy
```

원하는 포트로 실행:

```bash
./cproxy 8081
```

## 동작 확인

터미널 1에서 프록시 실행:

```bash
./cproxy 8080
```

터미널 2에서 프록시를 통해 HTTP 요청:

```bash
curl -x http://127.0.0.1:8080 http://example.com/
```

정상 동작하면 `cproxy`가 요청을 파싱하고 `example.com:80`으로 전달한 뒤 응답을 다시 `curl`로 relay합니다.

## 코드 흐름

1. `src/main.c`가 실행 포트를 결정하고 서버를 시작합니다.
2. `src/proxy.c`가 listening socket을 열고 클라이언트 연결을 accept합니다.
3. 연결마다 detached worker thread를 생성합니다.
4. worker는 HTTP 요청을 읽고 파싱합니다.
5. upstream 서버에 맞는 요청으로 재구성한 뒤 전달합니다.
6. upstream 응답을 클라이언트에게 relay하고 연결을 닫습니다.

## `mini-api-gateway-c`로 이어지는 지점

`cproxy`는 API gateway를 만들기 전에 필요한 가장 기본적인 네트워크 흐름을 다룹니다.

```text
Client -> cproxy -> Upstream Server -> cproxy -> Client
```

이 흐름을 이해하면 다음 단계에서 아래와 같은 API gateway 기능을 붙일 위치가 분명해집니다.

- routing: 요청 path나 host를 기준으로 upstream을 선택
- policy: 허용/차단 규칙, rate limit, 인증 전처리
- logging: 요청별 method, path, status, latency 기록
- metrics: 처리 건수, 실패율, upstream 응답 시간 수집
- health check: upstream 상태 확인과 장애 대응
- config: 코드 수정 없이 routing/policy를 바꿀 수 있는 설정 파일

따라서 `cproxy`는 "forward proxy의 핵심 요청 처리 흐름"을 정리한 프로젝트이고, `mini-api-gateway-c`는 그 흐름에 "서비스 운영에 필요한 gateway 책임"을 더하는 후속 프로젝트로 볼 수 있습니다.

## 문서

- [아키텍처 메모](docs/architecture.md)

## 포트폴리오 관점

`cproxy`는 단순 echo server를 넘어 다음 내용을 보여주기 좋은 프로젝트입니다.

- TCP 서버 생명주기와 socket API 사용
- HTTP 요청 구조에 대한 이해
- 동시 연결 처리를 위한 `pthread` 사용
- 파싱, upstream 연결, relay 로직의 모듈 분리
- 현재 한계와 확장 방향을 명확히 설명하는 문서화

포트폴리오 흐름에서는 `cproxy`를 먼저 제시해 네트워크 서버의 기본기를 보여주고, 이어서 `mini-api-gateway-c`에서 routing과 운영 기능을 추가하며 설계가 어떻게 확장되는지 설명할 수 있습니다.

## 향후 개선 아이디어

- `mini-api-gateway-c`에서 routing table과 설정 파일 구조 설계
- request body와 chunked transfer streaming 처리
- socket timeout과 graceful shutdown 추가
- 구조화 로그와 metrics 수집
- mock upstream 서버를 이용한 integration test 작성
- 필요 시 HTTPS `CONNECT` tunneling 또는 reverse proxy 방향 검토
