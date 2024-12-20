
# hashmap

This is a very simple hashmap implementation in C.

Features:
 - Support for any key and value type and custom hash functions
 - Full control over memory management
 - Very simple API
 - Very simple implementation (around 500 lines of code)
 - No dependencies

For most use cases, a very simple hashmap is sufficient.
For this reason, this hashmap is designed with the following goals in mind:
 - Ease of use
 - Ease of understanding
 - Ease of modification
 - Correctness

Non-Goals:
 - High performance
 - Thread safety

## Usage

Simply include the header file [`hashmap.h`](include/hashmap.h) in your project
and compile the implementation in [`src`](src) with your project.

## Documentation

The API is documented in the header file [`hashmap.h`](include/hashmap.h).

## Development

The library consists of a single header file ([`hashmap.h`](include/hashmap.h))
and the implementation in [`src`](src).

### Building

To build the project, run
```sh
make
```

Furthermore, you can set the following variables:
 - `BUILD`: Set the build type to `release` or `debug`. Default is `release`.
 - `CONSISTENCY_CHECKS`: If set to 1, enables full consistency checks on every
   hashmap operation. This is meant for debugging purposes and will make the
   hashmap incredibly slow.

#### Examples

Building in release mode without consistency checks (this is equivalent to
`make` without any arguments):
```sh
make BUILD=release
```

Building in debug mode with consistency checks enabled:
```sh
make BUILD=debug CONSISTENCY_CHECKS=1
```

### Testing

On every change, the code should be tested.
To completely test the code, the following commands should be executed.

Testing in release mode:
```sh
make clean && make test BUILD=release
```

Testing with consistency checks enabled:
```sh
make clean && make test CONSISTENCY_CHECKS=1 BUILD=debug
```

Testing with consistency checks disabled:
```sh
make clean && make test BUILD=debug
```

## License

```plaintext
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org/>
```

