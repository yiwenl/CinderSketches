# geom-parse-obj

[![stable](http://badges.github.io/stability-badges/dist/stable.svg)](http://github.com/badges/stability-badges)

Parse Wavefront .obj geometry file.

Notes:
- this parser support multiple meshes in one file via `g` sections in the OBJ file
- we return only vertex data, so no face normals and face tex coords

## Usage

[![NPM](https://nodei.co/npm/geom-parse-obj.png)](https://www.npmjs.com/package/geom-parse-obj)

#### `parseObj(str)`

Parameters:  
`str` - UTF8 encoded string with the contents of OBJ file

Returns:  
`geometry` or `[geometry, geometry, ...]` (depending if groups are present inside the file)

```javascript
geometry = {
  positions: [[x, y, z], [x, y, z], ...], // array of positions
  normals: [[x, y, z], [x, y, z], ...], // array of normals
  uvs: [[u, v], [u, v], ...], // array of tex coords
  cells: [[i, j, k], [i, j, k], ...] // array of triangles or polygons
}
```

## Example

```javascript
var parseObj = require('geom-parse-obj');
var fs = require('fs')
var objStr = fs.readFileSync(__dirname + '/geometry.obj', 'utf8')
var obj = parseObj(objStr)
console.log(obj.positions) // -> [[0, 0, 0], [1, 0, 1], ...]
```

## See also

[parse-obj](https://www.npmjs.com/package/parse-obj) by Mikola Lysenko

## License

MIT, see [LICENSE.md](http://github.com/vorg/geom-parse-obj/blob/master/LICENSE.md) for details.
