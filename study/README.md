# Ray Tracing: The Next Week — 학습 진행

> 교재: https://raytracing.github.io/books/RayTracingTheNextWeek.html
> 브랜치: `rudfo/raytracing-work`
> 소스: `src/TheNextWeek/`
> 학습 문서: `C:/Work/Claude/dashboard/docs/raytracing/`

## 챕터별 진행

| # | 챕터 | 핵심 파일 | 시간 | 상태 |
|---|------|----------|------|------|
| 01 | Overview | - | 0.5h | [ ] |
| 02 | Motion Blur | `camera.h`, `sphere.h` | 2h | [ ] |
| 03 | BVH | `aabb.h`, `bvh.h` | 3h | [ ] |
| 04 | Texture Mapping | `texture.h`, `rtw_stb_image.h` | 2h | [ ] |
| 05 | Perlin Noise | `perlin.h`, `texture.h` | 2.5h | [ ] |
| 06 | Quadrilaterals | `quad.h` | 1.5h | [ ] |
| 07 | Lights | `material.h`, `quad.h` | 1.5h | [ ] |
| 08 | Instances | `hittable.h` | 2h | [ ] |
| 09 | Volumes | `constant_medium.h` | 2h | [ ] |
| 10 | Final Scene | `main.cc` | 1.5h | [ ] |

**총 약 18.5시간 / 하루 1시간 → 3주**

## 학습 루틴 (챕터당)

1. 교재 읽기 → `chXX_*/notes.md`에 메모
2. 코드 직접 타이핑 (복붙 금지)
3. 빌드 → 렌더 → `chXX_*/renders/`에 저장
4. 학습 문서의 "이해도 체크" 셀프 점검

## 빌드

```bash
cd D:/04_Study/raytracing.github.io
cmake -B build
cmake --build build
./build/TheNextWeek > output.ppm
```
