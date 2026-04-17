# CMakeLists.txt 集成说明

在 `cpp/CMakeLists.txt` 中添加以下内容：

```cmake
# 数据库并发组件
set(DB_CONCURRENCY_SRC
    data/database/ConnectionPool.cpp
    data/database/WriteQueue.cpp
)

# 添加到主程序源文件
target_sources(stock_for_cpp PRIVATE
    ${DB_CONCURRENCY_SRC}
)

# 如果需要测试
if(BUILD_TESTS)
    add_executable(test_connection_pool 
        tests/data/database/test_connection_pool.cpp
        ${DB_CONCURRENCY_SRC}
    )
    target_link_libraries(test_connection_pool 
        sqlite3 
        ${CMAKE_THREAD_LIBS_INIT}
    )
endif()
```

---

# docker-compose.yml 集成说明

在 `docker-compose.yml` 中添加环境变量：

```yaml
services:
  stock-analysis:
    environment:
      - CPP_API_URL=http://localhost:3001
      - CPP_API_TIMEOUT=30000
```

---

# 集成完成后

运行测试：
```bash
cd cpp/build
cmake .. -DBUILD_TESTS=ON
make
./test_connection_pool
```

启动服务：
```bash
docker-compose up -d
```