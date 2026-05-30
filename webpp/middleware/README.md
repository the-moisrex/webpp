# Middleware and Events

The system unifies middleware pipelines, hooks, and lifecycle events under a common event model while remaining allocation-free and highly composable.

## Concepts

### Event

An event is a dispatch point that allows one or more handlers to participate in a workflow.

Web++ currently distinguishes between two kinds of events:

* Hooks
* Middlewares

### Hook

A hook is a broadcast-style event.

When triggered, every registered handler is executed in order.

Hooks are typically used for lifecycle notifications such as connection establishment, shutdown, cleanup, logging, metrics collection, and similar concerns.

### Middleware

A middleware is an intercepting event.

Unlike hooks, a middleware may decide whether execution should continue to subsequent handlers.

Two middleware models are supported:

#### Onion Middleware

The middleware explicitly decides when (or whether) the next middleware is invoked.

This model enables before/after execution semantics and forms the traditional "onion" execution flow used by modern web frameworks.

#### Two-Way Middleware

The middleware exposes `pre()` and/or `post()` interception points.

The framework automatically invokes downstream handlers between the two phases.

A `pre()` handler may optionally short-circuit execution.

### Event Tree

Handlers are represented internally as intrusive event nodes in a tree.

Each event category maintains its own event chain, allowing a single object to be linked into multiple independent event pipelines without additional allocations.

### Event Root

An event root represents the entry point of an event tree.

It owns the head nodes of all supported event chains and serves as the dispatch target for event execution.

### Dynamic Event Binding

The global `events` customization point provides dynamically scoped access to the currently active event root.

This allows event trees to be replaced, redirected, or isolated without coupling callers to a specific event root instance.

If we have multiple servers, this allows to dynamically change the `events` to point to the current server's event root before we call the middlewares and routes so the router when accessing `events` will always get the correct event/middleware tree.

## Design Goals

* Allocation-free event registration
* Lightweight middleware composition
* Support for multiple middleware paradigms
* Compile-time event definition
* Runtime event dispatch
* Intrusive storage
* Minimal dispatch overhead
* Extensible event categories
