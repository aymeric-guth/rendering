import os
import pathlib

vertices = [(0.0, 0.0, 0.0)]
triangles = []
OBJ_FILE = os.getenv("OBJ_FILE")
if not OBJ_FILE:
    raise FileNotFoundError


class Vec3:
    def __init__(self, raw, id):
        self.x, self.y, self.z = raw
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


with open(OBJ_FILE, "r") as f:
    raw = f.read().split("\n")
for i in raw:
    if not i:
        break
    if i[:2] == "vn" or i[:2] == "vt" or i[0] == "#" or i[0] == "s":
        continue
    if i[0] == "v":
        x, y, z = map(lambda x: float(x), i[2:].split(" "))
        vertices.append((x, y, z))
    elif i[0] == "f":
        (a, b, c) = i[2:].split(" ")
        a = int(a.split("/")[0])
        b = int(b.split("/")[0])
        c = int(c.split("/")[0])
        triangles.append((vertices[a], vertices[b], vertices[c]))
        # print(f"{a=}, {b=}, {c=}\n{vertices[a]=}, {vertices[b]=}, {vertices[c]=}")
    else:
        ...


p = pathlib.Path(__file__).parent
indent = "    "
f = open(p / "scene_data.h", "w")
f.write('#include "matrix.h"\n')
f.write('#include "types.h"\n\n')
f.write("#ifndef SCENE_DATA_H\n#define SCENE_DATA_H\n")
f.write(f"#define SCENE_SIZE {len(triangles)}\n\n")
f.write("static Tri _scene[] = {\n")
for i1, tri in enumerate(triangles):
    f.write(indent + "{\n")
    f.write(indent * 2 + ".v = {\n")
    for i2, v in enumerate(tri):
        f.write(indent * 3 + str(Vec3(v, i2)) + "\n")
    f.write(indent * 2 + "},\n")
    f.write(indent * 2 + ".c = COLOR_WHITE\n")
    f.write(indent + "},\n")
f.write("};\n\n")
f.write("#endif\n")
f.close()
