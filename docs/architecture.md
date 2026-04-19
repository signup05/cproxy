# Architecture Notes

## Goal

The goal of this project is to demonstrate how a simple socket-based proxy can be organized into a maintainable C project suitable for portfolio review.

## Execution Flow

1. `src/main.c`
   - reads the CLI port argument
   - starts the proxy server
2. `src/proxy.c`
   - creates the listening socket
   - accepts incoming clients
   - spawns one detached worker thread per client
   - coordinates request parsing, upstream connection, and response relay
3. `src/http.c`
   - reads the inbound request
   - parses request metadata such as method, host, port, and path
   - rebuilds the upstream request in origin-form
   - connects to the destination server
   - relays the upstream response

## Design Choices

### Thread-per-connection

This model is simple and readable for a portfolio project. It is not the most scalable architecture, but it clearly demonstrates concurrency with `pthread`.

### Request normalization

Forward proxies often receive absolute-form targets such as:

```http
GET http://example.com/index.html HTTP/1.1
```

Origin servers generally expect:

```http
GET /index.html HTTP/1.1
```

This project converts the request before forwarding it upstream.

### Error handling

The proxy returns minimal HTTP error responses for malformed requests or upstream failures. This makes the tool easier to test and easier to explain during review.

## Current Limitations

- No HTTPS tunneling
- No full request-body streaming
- No connection pooling
- No epoll-based event loop

These are good next-step talking points during interviews because they show awareness of production tradeoffs.
