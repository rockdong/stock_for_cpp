## Why

Express 路由按定义顺序匹配，导致 `/analysis/:code` 拦截了所有 `/analysis/process/*` 路由。

## What Changes

重新组织 API 路由顺序，确保具体路径在参数化路径之前匹配。

## Impact

- nodejs/src/api.js - 路由定义顺序调整