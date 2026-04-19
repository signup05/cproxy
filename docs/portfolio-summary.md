# Portfolio Summary

## Project Title

Linux Environment Proxy Server Implementation in C

## Summary

This project implements a multithreaded HTTP proxy server in C using POSIX sockets and `pthread`. It was refactored from a single-file prototype into a modular project with clearer separation of concerns, stronger request parsing, and portfolio-oriented documentation.

## What This Project Demonstrates

- understanding of TCP socket server fundamentals
- concurrent request handling with threads
- practical HTTP header parsing
- translation of proxy-style requests into upstream-safe requests
- structured error handling
- modular C project organization
- technical writing and architectural explanation

## Improvement Points Over the Original Version

- split the single-file implementation into reusable modules
- improved socket setup and bind fallback logic
- replaced one-shot response reads with full relay loop behavior
- added input validation and HTTP error responses
- documented scope, limitations, and extension paths

## Interview Talking Points

- why the project chose thread-per-connection first
- how absolute URI proxy requests differ from origin-form requests
- where HTTPS `CONNECT` support would be inserted
- why request-body streaming matters for real proxy servers
- how the project could evolve toward `select`, `poll`, or `epoll`
