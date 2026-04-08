#ifndef AABB_H
#define AABB_H

// AABB (Axis-Aligned Bounding Box) — 축 정렬 바운딩 박스
// 3개 축(x,y,z)의 구간(interval)으로 정의되는 직육면체.
// BVH에서 레이가 오브젝트 그룹을 빗나가는지 빠르게 판정하는 용도.
class aabb {
public:
	interval x, y, z; // 각 축의 [min, max] 구간

	aabb() {} // 기본: 빈 AABB

	// 3개 구간으로 직접 생성
	aabb(const interval& x, const interval& y, const interval& z)
		: x(x), y(y), z(z) {
	}

	// 두 꼭짓점(대각선 양 끝)으로 생성
	// a,b 순서 상관없이 자동으로 min/max 정렬
	aabb(const point3& a, const point3& b)
	{
		x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
		y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
		z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
	}

	aabb(const aabb& box0, const aabb& box1)
	{
		x = interval(box0.x, box1.x);
		y = interval(box0.y, box1.y);
		z = interval(box0.z, box1.z);
	}

	// 인덱스로 축 구간 접근 (0=x, 1=y, 2=z)
	const interval& axis_interval(int n) const {
		if (n == 1) return y;
		if (n == 2)  return z;
		return x;
	}

	// Slab Method — 레이-AABB 교차 판정
	//
	// 원리: 각 축마다 레이가 slab(평행한 두 평면 사이)을 관통하는 t 구간을 구함
	//       3축 구간의 교집합이 유효하면 hit
	//
	//   X축 slab: [t0_x, t1_x]
	//   Y축 slab: [t0_y, t1_y]
	//   Z축 slab: [t0_z, t1_z]
	//   교집합 = max(t0들) ~ min(t1들) → 유효하면 hit
	bool hit(const ray& r, interval ray_t) const {
		const point3& ray_orig = r.origin();
		const vec3& ray_dir = r.direction();

		for (int axis = 0; axis < 3; axis++)  // ← 원본에 오타 있었음: ':' → ';'
		{
			const interval& ax = axis_interval(axis);
			const double adinv = 1.0 / ray_dir[axis]; // 역수 한 번만 계산 (나눗셈 회피)

			// 이 축의 slab 진입/이탈 t값
			auto t0 = (ax.min - ray_orig[axis]) * adinv;
			auto t1 = (ax.max - ray_orig[axis]) * adinv;

			// t0 < t1이면 정방향, 아니면 레이가 음의 방향
			if (t0 < t1)
			{
				if (t0 > ray_t.min) ray_t.min = t0; // 교집합 진입점 갱신
				if (t1 < ray_t.max) ray_t.max = t1; // 교집합 이탈점 갱신
			}
			else {
				if (t1 > ray_t.min) ray_t.min = t1;
				if (t0 < ray_t.max) ray_t.max = t0;
			}

			// 교집합이 빈 구간 → miss
			if(ray_t.max <= ray_t.min)
				return false;
		}
		return true;
	}

	int longest_axis() const {
		if (x.size() > y.size())
		{
			return x.size() > z.size() ? 0 : 2;
		}
		else {
			return y.size() > z.size() ? 1 : 2;
		}

		
	}
	static const aabb empty, universe;
};

const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);

#endif