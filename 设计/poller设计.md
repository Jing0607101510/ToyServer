# 作用

- 封装了epoll_wait。增加，删除，修改，设置等。
- 封装了对timer的处理。（含有timer管理类）
- 不是直接操作fd，而是操作channel