# Design, Practices, and Principles

## General

### External library Types

Unless strickly necessary, types by external libraries should not be used. If necessary the use should be kept a function or file level scope.

### Module level headers

Modules should have a single external header. The name should match the name of the module. Functions meant to be internal to the module should be declared within the module's config header.

Example:
`render/render_config.h`

## Engine
