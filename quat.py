import math


def q_conjugate(q):
    w, x, y, z = q
    return (w, -x, -y, -z)


def qv_mult(q1, v1):
    q2 = (0.0,) + v1
    return q_mult(q_mult(q1, q2), q_conjugate(q1))[1:]


def q_mult(q1, q2):
    w1, x1, y1, z1 = q1
    w2, x2, y2, z2 = q2
    w = w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2
    x = w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2
    y = w1 * y2 + y1 * w2 + z1 * x2 - x1 * z2
    z = w1 * z2 + z1 * w2 + x1 * y2 - y1 * x2
    return w, x, y, z


def euler_to_quaternion(phi, theta, psi):
    qw = math.cos(phi / 2) * math.cos(theta / 2) * math.cos(psi / 2) + math.sin(
        phi / 2
    ) * math.sin(theta / 2) * math.sin(psi / 2)
    qx = math.sin(phi / 2) * math.cos(theta / 2) * math.cos(psi / 2) - math.cos(
        phi / 2
    ) * math.sin(theta / 2) * math.sin(psi / 2)
    qy = math.cos(phi / 2) * math.sin(theta / 2) * math.cos(psi / 2) + math.sin(
        phi / 2
    ) * math.cos(theta / 2) * math.sin(psi / 2)
    qz = math.cos(phi / 2) * math.cos(theta / 2) * math.sin(psi / 2) - math.sin(
        phi / 2
    ) * math.sin(theta / 2) * math.cos(psi / 2)

    return [qw, qx, qy, qz]


def quaternion_to_euler(w, x, y, z):
    t0 = 2 * (w * x + y * z)
    t1 = 1 - 2 * (x * x + y * y)
    X = math.atan2(t0, t1)

    t2 = 2 * (w * y - z * x)
    t2 = 1 if t2 > 1 else t2
    t2 = -1 if t2 < -1 else t2
    Y = math.asin(t2)

    t3 = 2 * (w * z + x * y)
    t4 = 1 - 2 * (y * y + z * z)
    Z = math.atan2(t3, t4)

    return X, Y, Z


v1 = (1, 0, 0)
phi = math.pi / 2
theta = math.pi / 4
psi = math.pi / 2
q = euler_to_quaternion(phi, theta, psi)
print("w =", q[0])
print("x =", q[1])
print("y =", q[2])
print("z =", q[3])

v2 = qv_mult(q, v1)
print(v1)
print(v2)
