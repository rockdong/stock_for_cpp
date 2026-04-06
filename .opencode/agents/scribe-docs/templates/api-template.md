# API 文档模板

## 接口信息
- **名称:** 
- **路径:** 
- **方法:** GET/POST/PUT/DELETE
- **版本:** v1
- **状态:** 设计中/开发中/已发布/已废弃

---

## 概述

[简要描述接口用途]

---

## 请求

### 请求头

| 参数 | 类型 | 必填 | 描述 |
|------|------|------|------|
| Authorization | string | 是 | Bearer Token |
| Content-Type | string | 是 | application/json |

### 路径参数

| 参数 | 类型 | 必填 | 描述 |
|------|------|------|------|
| id | string | 是 | 资源ID |

### 查询参数

| 参数 | 类型 | 必填 | 描述 | 默认值 |
|------|------|------|------|--------|
| page | integer | 否 | 页码 | 1 |
| size | integer | 否 | 每页数量 | 20 |

### 请求体

```json
{
  "field1": "string",
  "field2": 0,
  "field3": {
    "nested": "string"
  }
}
```

### 请求示例

```bash
curl -X POST "https://api.example.com/v1/resource" \
  -H "Authorization: Bearer <token>" \
  -H "Content-Type: application/json" \
  -d '{
    "field1": "value1",
    "field2": 100
  }'
```

---

## 响应

### 成功响应 (200 OK)

```json
{
  "code": 200,
  "message": "success",
  "data": {
    "id": "xxx",
    "field1": "value1",
    "field2": 100,
    "createdAt": "2024-01-01T00:00:00Z"
  }
}
```

### 错误响应

#### 400 Bad Request

```json
{
  "code": 400,
  "message": "参数错误",
  "errors": [
    {
      "field": "field1",
      "message": "field1 不能为空"
    }
  ]
}
```

#### 401 Unauthorized

```json
{
  "code": 401,
  "message": "未授权"
}
```

#### 404 Not Found

```json
{
  "code": 404,
  "message": "资源不存在"
}
```

---

## 数据模型

### Resource 对象

| 字段 | 类型 | 描述 |
|------|------|------|
| id | string | 资源唯一标识 |
| field1 | string | 字段描述 |
| field2 | integer | 字段描述 |
| createdAt | string | 创建时间 (ISO 8601) |
| updatedAt | string | 更新时间 (ISO 8601) |

---

## 注意事项

- [注意事项1]
- [注意事项2]

---

## 变更记录

| 日期 | 版本 | 变更内容 |
|------|------|----------|
| | | |