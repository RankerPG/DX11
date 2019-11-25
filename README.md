# DX11
>내용 정리
<pre>
목표는 12.15 까지 다 읽기.
며칠 진행해보고 전체적인 일정 정하기.
</pre>

## 19.11.25
<code> ~ 67 page </code>

### vector
- 직교화 : 그람 슈미트 직교화
<pre><사진추가></pre>

- 2차원 벡터의 수직인 벡터를 구하는 방법 : u(ux,uy) ┴ v(-uy, ux) = u·v = ux * -uy + uy * ux

- 외적을 통한 직교화
1. v0을 정규화 ( w0 )
2. w0 X v1을 정규화 ( w2 )
3. w2 X w0 ( w1 )

### XNA Math

- SSE2 ( streaming SIMD extentions 2 )

- SIMD ( single instruction multy data )

- XMVECTOR에 적재하고 저장하는 함수들
1. XMLoadFloat : 적재
2. XMStoreFloat : 저장
3. XMSetFloat : 벡터의 1개 요소 적재
4. XMGetFloat : 벡터의 1개 요소 저장

```cpp

// 32bit window
typedef const XMVECTOR FXMVECTOR
typedef const XMVECTOR& CXMVECTOR

// 64bit window
typedef const XMVECTOR& FXMVECTOR
typedef const XMVECTOR& CXMVECTOR
```

- 함수는 매개변수로 XMVECTOR를 사용할 때 처음 3개는 FXMVECTOR, 다음 3개는 CXMVECTOR를 사용해야 하는
규칙을 지켜야 한다.

- 상수 벡터는 XMVECTORF32를 사용하고 즉시 초기화할 때 사용






















