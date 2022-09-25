import json
import pathlib


class Vec3:
    def __init__(self, raw, id):
        self.x = float(raw.get("x"))
        self.y = float(raw.get("y"))
        self.z = float(raw.get("z"))
        self.id = id

    def __repr__(self):
        return f"Vec3(x={self.x}, y={self.y}, z={self.z})"

    def __str__(self):
        s = f".x = {self.x}f, .y = {self.y}f, .z = {self.z}f"
        return "{ " + s + " },"


class Tri:
    def __init__(self, id):
        self.id = id

    def __str__(self):
        return f"Tri t{self.id}"


p = pathlib.Path(__file__).parent
with open(p / "scene_data.json", "r") as f:
    raw = f.read()
scene = json.loads(raw)

indent = "    "
f = open(p / "scene_data.h", "w")
f.write('#include "matrix.h"\n')
f.write('#include "types.h"\n\n')
f.write("#ifndef SCENE_DATA_H\n#define SCENE_DATA_H\n")
f.write(f"#define SCENE_SIZE {len(scene)}\n\n")
f.write("static Tri _scene[] = {\n")
for i1, tri in enumerate(scene):
    f.write(indent + "{\n")
    f.write(indent * 2 + ".v = {\n")
    for i2, v in enumerate(tri):
        f.write(indent * 3 + str(Vec3(v, i2)) + "\n")
    f.write(indent * 2 + "},\n")
    f.write(indent * 2 + f".c = {v.get('color')}\n")
    f.write(indent + "},\n")
f.write("};\n\n")
f.write("#endif\n")
f.close()
