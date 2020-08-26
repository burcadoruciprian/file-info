# File Info

Native node module for reading file version information on Windows

It uses the [GetFileVersionInfo](https://msdn.microsoft.com/en-us/library/windows/desktop/ms647003(v=vs.85).aspx)
function. For now only the **FileVersion** is returned

## Installation

```bash
npm install winfileinfo
```

## Usage 

```js
const fileInfo = require('winfileinfo/winfileinfo.node');
console.log(fileInfo.getFileVersion('path_to_file'));
```
