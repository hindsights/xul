### README

#### Overview
XUL(auXiliary Utility Library) is a collection of utility functions and classes in c++, which provides facilities such as networking, logging, input/output stream, serialization, option parsing, encoding, etc, and also the corresponding lua bindings.
* networking: a small asynchronous network/io framework based on boost.asio<br>
It contains implementations for udp/tcp socket, http server/client/connection and other functionalities.
* logging: a tiny logging library<br>
The logging process can be customized through startup options or configuration file, and the parameters can also be modified at runtime. The logging messages can be output to console, or a rolling file, or system log(like android's log).
* io/serialization: input and output stream, data serialization...<br>
It could be useful when implementing protocols.
* system utilities and wrappers of os functionalities such as path, file, file_system, os module, os process, pipe, stack backtracing, etc.

#### Examples
* httpsvr<br>
A simple http server which demonstrates how file download and file upload are implemented by using xul's networking framework.

* lua bindings<br>
xlua is a lua interpreter with xul embedded.<br>
luaxul is a lua extension library.<br>
httpsvr.lua demonstrates how to implement file download and file upload in lua by using xul's lua bindings.<br>
