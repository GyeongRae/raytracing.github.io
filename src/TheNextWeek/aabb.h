#ifndef AABB_H
#define AABB_H
//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================


// AABB (Axis-Aligned Bounding Box) — 축 정렬 바운딩 박스
// 3개 축(x,y,z)의 구간(interval)으로 정의되는 직육면체.
// BVH 가속 구조에서 레이가 오브젝트 그룹을 빗나가는지 빠르게 판정하는 데 사용.
class aabb {
  public:
    interval x, y, z; // 각 축의 [min, max] 구간

    aabb() {} // 기본 생성자: 빈 AABB (interval이 기본적으로 empty이므로)

    // 3개 구간으로 직접 생성
    aabb(const interval& x, const interval& y, const interval& z)
      : x(x), y(y), z(z)
    {
        pad_to_minimums(); // 너무 얇은 축은 패딩 (수치 안정성)
    }

    // 두 꼭짓점(대각선 양 끝)으로 생성
    // a, b의 min/max 순서를 신경 쓸 필요 없도록 자동 정렬
    aabb(const point3& a, const point3& b) {
        x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);

        pad_to_minimums();
    }

    // 두 AABB를 감싸는 합집합 AABB 생성
    // BVH 트리에서 자식 노드들의 AABB를 합칠 때 사용
    aabb(const aabb& box0, const aabb& box1) {
        x = interval(box0.x, box1.x);
        y = interval(box0.y, box1.y);
        z = interval(box0.z, box1.z);
    }

    // 인덱스로 축 구간 접근 (0=x, 1=y, 2=z)
    // BVH 분할 시 축을 동적으로 선택할 때 유용
    const interval& axis_interval(int n) const {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    // Slab Method — 레이-AABB 교차 판정
    // 각 축마다 레이가 slab(평행한 두 평면 사이)을 관통하는 t 구간을 구하고,
    // 3축 구간의 교집합이 유효하면 hit.
    //
    // 핵심 수학:
    //   t0 = (slab_min - ray_origin) / ray_direction   ← 진입 t
    //   t1 = (slab_max - ray_origin) / ray_direction   ← 이탈 t
    //   3축 교집합: t_enter = max(t0_x, t0_y, t0_z), t_exit = min(t1_x, t1_y, t1_z)
    //   hit ⟺ t_enter < t_exit
    bool hit(const ray& r, interval ray_t) const {
        const point3& ray_orig = r.origin();
        const vec3&   ray_dir  = r.direction();

        for (int axis = 0; axis < 3; axis++) {
            const interval& ax = axis_interval(axis);
            const double adinv = 1.0 / ray_dir[axis]; // 역수 한 번만 계산 (나눗셈 회피)

            // 이 축의 slab 진입/이탈 t값 계산
            auto t0 = (ax.min - ray_orig[axis]) * adinv;
            auto t1 = (ax.max - ray_orig[axis]) * adinv;

            // t0 < t1이면 정방향, 아니면 레이가 음의 방향 → swap 대신 분기
            if (t0 < t1) {
                if (t0 > ray_t.min) ray_t.min = t0; // 교집합 진입점 갱신
                if (t1 < ray_t.max) ray_t.max = t1; // 교집합 이탈점 갱신
            } else {
                if (t1 > ray_t.min) ray_t.min = t1;
                if (t0 < ray_t.max) ray_t.max = t0;
            }

            // 교집합이 빈 구간이면 → miss (이 축에서 이미 벗어남)
            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }

    // 가장 긴 축의 인덱스 반환 (0=x, 1=y, 2=z)
    // BVH 분할 시 가장 긴 축으로 나누면 공간 분할 효율이 좋음
    int longest_axis() const {
        if (x.size() > y.size())
            return x.size() > z.size() ? 0 : 2;
        else
            return y.size() > z.size() ? 1 : 2;
    }

    static const aabb empty, universe;

  private:

    // 너무 얇은 AABB 보정 (예: 평면 오브젝트)
    // 두께가 delta 미만이면 양쪽으로 확장.
    // 이유: 두께 0인 slab에서 부동소수점 오차로 교차 판정이 불안정해짐
    void pad_to_minimums() {
        double delta = 0.0001;
        if (x.size() < delta) x = x.expand(delta);
        if (y.size() < delta) y = y.expand(delta);
        if (z.size() < delta) z = z.expand(delta);
    }
};

// 미리 정의된 상수: 빈 AABB, 무한 AABB
const aabb aabb::empty    = aabb(interval::empty,    interval::empty,    interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);

// AABB + 오프셋 = 이동된 AABB (Instance translate에서 사용)
aabb operator+(const aabb& bbox, const vec3& offset) {
    return aabb(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}

aabb operator+(const vec3& offset, const aabb& bbox) {
    return bbox + offset;
}


#endif
