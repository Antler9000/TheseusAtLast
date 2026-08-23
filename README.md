# PodoNatureEngine
HLSL, D3D12, Win32, C++을 이용하여 구현 중인 실시간 3D 엔진입니다.

사용자가 가상 자연환경을 구성할 수 있도록 하는 것을 목표로 하고 있습니다.

현재 기초적인 렌더링 기능을 구현하고 있습니다.

<br>




<!---------------------------------------------------------------------------------------------------------------------------------------------->
## 목차
- [1. 스크린샷](#1-스크린샷)
- [2. 실행 방법](#2-실행-방법)
- [3. 주요 기능](#3-주요-기능)
- [4. 성능 측정](#4-성능-측정)
- [5. 구현 예정](#5-구현-예정)
- [6. 구현 제외](#6-구현-제외)
- [7. 외부 항목](#7-외부-항목)
- [8. 구현 참고](#8-구현-참고)

<br>




<!---------------------------------------------------------------------------------------------------------------------------------------------->
## 1. 스크린샷
[로딩]
![로딩 스크린샷](Docs/screenshot_1.png)

[실행]
![실행 스크린샷](Docs/screenshot_2.png)

[메뉴]
![메뉴 스크린샷](Docs/screenshot_3.png)

<br>




<!---------------------------------------------------------------------------------------------------------------------------------------------->
## 2. 실행 방법
### 2.1. 최소 사양
|구분           |최소 사양                            |
|---------------|-------------------------------------|
|운영체제       |Windows 10 22H2 64-bit 이상          |
|CPU            |미정                                 |
|GPU 피처 레벨  |D3D_FEATURE_LEVEL_12_0 이상          |
|GPU 셰이더 모델|Shader Model 6.6 이상                |
|RAM            |미정                                 |
|저장 장치      |미정                                 |

- 추후 바인드리스 서술자 힙 접근을 사용할 예정이기 때문에 Shader Model 6.6을 최소 사양으로 정하였습니다. 운영체제는 지원 범위를 단순화하기 위해 Windows 10의 마지막 기능 업데이트인 22H2를 최소 지원 버전으로 정하였습니다.

- HDR 제시, 레이 트레이싱, 메시 셰이더 등의 옵션은 추가 사양이 필요할 수 있습니다. 실행 환경이 해당 옵션을 지원하지 않는다면 체크박스가 비활성화됩니다.

<br>




### 2.2. 빌드 방법
1. 리포지토리를 다운로드합니다.

2. 다운로드한 리포지토리에 포함된 `PodoNatureEngine.slnx` 솔루션 파일을 더블 클릭합니다.

3. Visual Studio 상단의 구성을 `Release`로, 플랫폼을 `x64`로 설정합니다.  

4. Visual Studio 상단에서 빌드(B)의 솔루션 빌드(Ctrl+Shift+B)를 누릅니다.

5. `PodoNatureEngine/OutDir`에서 `PodoNatureEngineRelease64.exe` 이름의 파일이 생성되었다면 빌드에 성공한 것입니다.

<br>




### 2.3. 실행 방법
1. 위 빌드 과정을 통해 생성한 `PodoNatureEngineRelease64.exe`를 더블 클릭하여 실행합니다.

2. 로딩이 완료되면 Click here to start 버튼이 중앙 하단에 나타납니다. 이를 눌러 시작합니다.

3. 마우스 우클릭을 통해 시야의 방향을 회전시킵니다. WASD 키로 시야의 위치를 이동시킵니다(미구현).

4. 마우스 좌클릭을 통해 월드를 조작할 수 있습니다(미구현).

5. 왼쪽 상단의 Menu 버튼을 누르면 메뉴와 함께 옵션이 나타납니다.

6. 종료하고 싶으면 메뉴의 Exit 버튼을 누르거나 ALT+F4 단축키를 누르면 됩니다. 



<br>




<!---------------------------------------------------------------------------------------------------------------------------------------------->
## 3. 주요 기능
### 3.1. 조형 기능
아직 구현하지 않았습니다.

<br>




### 3.2. 조명 기능
아직 구현하지 않았습니다.

<br>




### 3.3. 조작 기능
|입력                         |기능                     |
|-----------------------------|-------------------------|
|마우스 좌클릭                |월드 조작(미구현)        |
|마우스 우클릭 + 이동         |시야 회전(미구현)        |
|마우스 휠                    |시야 앞뒤 이동(미구현)   |
|키보드 WASD                  |시야 이동(미구현)        |
|키보드 ESC                   |실행<->메뉴 간 상태 전환 |
|키보드 ALT+ENTER             |창 모드 전환             |
|키보드 ALT+F4                |종료                     |
|창 끌기                      |창 위치 이동             |
|창 모서리 끌기               |창 크기 조절             |

<br>




### 3.4. 옵션 기능
|옵션                   |설정값                                       |
|-----------------------|---------------------------------------------|
|창 모드                |창 모드 or 테두리 없는 창 모드               |
|VSync                  |Off or On                                    |
|Tearing                |피처 지원 여부와 VSync 여부에 따라 자동 적용 |
|HDR(일부구현)          |Off or On                                    |
|레이 트레이싱(미구현)  |Off or On                                    |
|메시 셰이더(미구현)    |Off or On                                    |
|GUI 크기               |50% or 75% or 100% or 125% or 150%           |

- 옵션은 `PodoNatureEngine/SavedSettings.txt` 파일에 저장되며, 다음 실행 시 복원됩니다.

<br>




<!---------------------------------------------------------------------------------------------------------------------------------------------->
## 4. 성능 측정
### 4.1. 측정 환경
|환경 요소          |세부 정보                                                        |
|-------------------|-----------------------------------------------------------------|    
|CPU                |i5-13600KF, 3500 MHz, 14 코어, 20 논리 프로세서                  |
|GPU                |INNO3D GeForce RTX 4070 Ti D6X 12GB X3                           |
|Display            |Samsung Odyssey G6 LS32BG650 QHD 240 Hz                          |
|RAM                |32 GB, DDR4, 3600 Hz                                             |
|OS                 |Windows 11, 버전 25H2, 빌드 26200.8973                           |
|IDE                |Microsoft Visual Studio Community 2026 (64 - bit) 버전 18.8.2    |
|플랫폼 도구 집합   |v145 for Microsoft C++ Build Tools                               |
|컴파일러 버전      |x86용 Microsoft (R) C/C++ 최적화 컴파일러 버전 19.51.36252       |
|구성               |`Profile` (= 기본 `Release` 구성 + `USE_PIX` 전처리기 상수 정의) |

<br>





### 4.2. 측정 방식
PIX의 Sequential Timing Capture를 이용하여 업데이트 로직의 구간을 측정합니다.

Metrics 값을 통해 각 구간의 소요 시간을 확인합니다.

측정은 테두리 없는 창 모드 상태에서 VSync를 끈 경우와 켠 경우로 나누어 캡처합니다.

캡처 시작 및 종료 버튼을 누르기 위해 PIX와 애플리케이션 사이의 화면 전환 시점이 캡처에 포함되어 있습니다.

<br>




### 4.3. 측정 결과
[VSync Off]
|구분 |측정 구간                                                                |평균 소요 시간 |
|-----|-------------------------------------------------------------------------|--------------:|
|CPU  |프레임 시간                                                              |450,894 ns     |
|     |├─논-렌더 로직                                                           |283 ns         |
|     |└─ 렌더 로직                                                             |450,397 ns     |
|     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─ 커맨드 리스트 초기화  |114,628 ns     |
|     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─ 자원 바인딩           |9,368 ns       |
|     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─ 씬 그리기             |320 ns         |
|     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─ GUI 그리기            |13,990 ns      |
|     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─ 자원 언바인딩         |1,638 ns       |
|     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─ 커맨드 리스트 제출    |73,924 ns      |
|     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;└─ 제시                  |235,626 ns     |
|GPU  |프레임 시간                                                              |39,156 ns      |
|     |├─ 자원 바인딩                                                           |38,322 ns      |
|     |├─ 씬 그리기                                                             |9 ns           |
|     |├─ GUI 그리기                                                            |763 ns         |
|     |└─ 자원 언바인딩                                                         |12 ns          |

- 총 캡처 시간: 9,839.76 ms

- GPU 명령이 모두 소진될 때까지 CPU를 대기시키는 현 로직으로 인해, 커맨드 리스트 초기화 과정에서 CPU 프레임 시간의 약 25%가 대기 상태로 낭비되고 있습니다.

- 기하 그리기가 구현되지 않았으므로, GPU의 연산 대부분은 백 버퍼의 색상을 초기화하는 로직이 포함된 자원 바인딩 구간에서 발생하고 있습니다.

<br>

[VSync On]  
|구분 |측정 구간                                                                |평균 소요 시간 |
|-----|-------------------------------------------------------------------------|--------------:|
|CPU  |프레임 시간                                                              |4,161,893 ns   |
|     |├─논-렌더 로직                                                           |702 ns         |
|     |└─ 렌더 로직                                                             |4,160,713 ns   |
|     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─ 커맨드 리스트 초기화  |3,573,808 ns   |
|     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─ 자원 바인딩           |24,925 ns      |
|     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─ 씬 그리기             |646 ns         |
|     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─ GUI 그리기            |35,617 ns      |
|     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─ 자원 언바인딩         |3,192 ns       |
|     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;├─ 커맨드 리스트 제출    |153,068 ns     |
|     |&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;└─ 제시                  |368,147 ns     |
|GPU  |프레임 시간                                                              |192,746 ns     |
|     |├─ 자원 바인딩                                                           |190,587 ns     |
|     |├─ 씬 그리기                                                             |7 ns           |
|     |├─ GUI 그리기                                                            |2,083 ns       |
|     |└─ 자원 언바인딩                                                         |10 ns          |

- 총 캡처 시간: 8,973.71 ms

- VSync를 켰으므로 명령 큐에 제출된 Present 명령은 VSyncBlank 타이밍에 처리되고, 이 시점까지 `FlushCommandQueue(..)`가 CPU의 다음 프레임 렌더 시작을 블록하므로, CPU의 "프레임 시간"과 "이전 명령 완료 대기" 시간이 늘어납니다.

- 측정 환경의 모니터 주사율은 240Hz이므로 주기는 $\frac{1}{240\ \mathrm{Hz}} \approx 4.17\ \mathrm{ms}$이고 이는 CPU의 "프레임 시간" 측정값과 거의 맞아떨어집니다.

- VSync에 직접적인 영향을 받지 않는 연산 시간이 2배로 늘어난 이유는, VSync에 의한 유휴 시간이 증가하며 CPU와 GPU의 스케줄링이나 클럭이 영향을 받았을 가능성을 생각할 수 있으나, 현재 측정만으로는 특정할 수 없습니다.

<br>




<!---------------------------------------------------------------------------------------------------------------------------------------------->
## 5. 구현 예정
- **초기화 기능**
  - 재초기화 단계 분할

<br>

- **단일 PSO 기능**
  - 에셋 로드, 텍스처 로드
  - 텍스처 자원
  - SRV
  - 업로드 힙 자원 관리(`DirectX::GraphicsMemory`)
  - (!)오브젝트의 world 업데이트
  - (!)그리기 명령
  - 조명, 텍스처 매핑, 노말 매핑, 거리별 안개 셰이더
  - SDR 감마 인코딩-디코딩(SRGB RTV 사용)
  - 카메라 구조체, 카메라 조작 기능
  - 오브젝트 배치, 선택 기능

<br>

- **단일 PSO 최적화**
  - 대상 경량화             : 사전 생성 LOD, 인덱스 타입 크기 구분
  - 로딩 대상 선별          : 로딩 정책
  - 로딩 로직 최적화        : 런타임 로딩 전용 파일 포맷
  - 로딩과 렌더 간 병렬화   : 파일 읽기용 CPU 스레드, 로딩 완료 버튼 활성화 조건
  - 업로드 대상 선별        : 업로드 힙 자원 관리 정책, 디폴트 힙 자원 관리 정책
  - 업로드 로직 최적화      : `DirectX::ResourceUploadBatch`, `DirectX::GraphicsMemory` 비용 검증
  - 업로드와 렌더 간 병렬화 : Copy Queue
  - 루트 서명 직접화        : 루트 서술자, 루트 상수 
  - 루트 서명 간접화        : 서술자 테이블, 셰이더 자원 테이블 간접 인덱싱, 바인드리스 서술자 힙 접근
  - 루트 서명 정렬          : 변경 횟수가 많은 것부터 내림차순으로 정렬
  - 그리기 대상 선별        : 오브젝트, 광원 절두체 선별
  - 그리기 명령 횟수 축소   : 인스턴싱
  - 그리기 명령 일련화      : 해쉬가 아닌 어레이로 Object를 저장
  - 그리기 명령 정렬        : 캐싱 비용이 기준으로 그리기 명령을 정렬
  - 그리기 명령 병렬화      : 스레드별 명령 리스트, 명령 할당자
  - 그리기 명령 재사용      : 번들
  - 셰이더 최적화           : 분기 수 조절, 로컬리티 개선 등
  - CPU와 GPU 간 병렬화     : 명령 할당자 순환 배열, 업로드 대기 시점 확인 및 미루기

<br>

- **다중 PSO 기능**
  - 조형 PSO  : 빌보딩(초목), 변위 매핑(지면, 수면), 스킨 애니메이션, 파티클
  - 조명 PSO  : 블랜딩, 정적 환경 매핑, 동적 환경 매핑, 그림자 매핑, SSAO, 레이 트레이싱, 복셀 전역 조명

<br>

- **다중 PSO 최적화**
  - 비교          : 포워드 렌더링 vs 포워드+ 렌더링 vs 지연 렌더링
  - 비교          : 네이티브 렌더링 vs VRS
  - 비교          : 사전 생성 LOD vs 기하 셰이더 런타임 LOD vs 테셀레이션 런타임 LOD vs 메시 셰이더 런타임 LOD
  - 비교          : 정점 셰이더 빌보딩 vs 기하 셰이더 빌보딩 vs 메시 셰이더 빌보딩
  - 비교          : 단일 렌더 타깃 동적 입방체 그리기 vs 다중 렌더 타깃 어레이 동적 입방체 그리기(`WithoutGSEmulation`)
  - 비교          : 컴퓨트 셰이더 파티클 vs 메시 셰이더 파티클
  - PSO 내 최적화 : 루트 서명 직접화, 루트 서명 간접화, 루트 서명 정렬, 셰이더 최적화
  - PSO 간 최적화 : Placed Resource 공간 활용, 입력 슬롯 분할, PSO 변경 횟수 절약, 루트 서명 표준화

<br>

- **제시 개선**
  - 지연 한도 설정(`SetMaximumFrameLatency`, `WaitableObject`)
  - HDR 제시(톤 매핑, 색역 매핑, PQ 전달 함수)
  - 백 버퍼, GUI 해상도와 독립적인 렌더 해상도 조절 기능

<br>




<!---------------------------------------------------------------------------------------------------------------------------------------------->
## 6. 구현 제외
- **디바이스 소실 감지**  
  디바이스 소실을 감지하는 로직은 구현을 단순화하기 위해 작성하지 않았습니다.

<br>

- **윈도우 DPI 설정값 자동 반영**  
  본 엔진은 DPI 설정값에 의한 윈도우의 자동 스케일링으로 화면이 훼손되지 않도록 DPI AWARENESS를 매니페스트에 명시하였습니다. 윈도우의 DPI 설정값은 GUI 크기에 반영하지 않고, 대신 옵션에서 GUI의 배율을 50%, 75%, 100%, 125%, 150% 중에서 선택할 수 있도록 간단히 구현하였습니다.

<br>

- **안티 앨리어싱**  
  대중화된 QHD, UHD 해상도에 적합한 안티 앨리어싱 기법은 프로젝트 규모를 제한하고자 제외하였습니다.

<br>

- **오클루전 선별**  
  프로젝트 규모를 제한하고자 제외하였습니다.

<br>




<!---------------------------------------------------------------------------------------------------------------------------------------------->
## 7. 외부 항목
개발 및 빌드를 위해 다음 SDK, 툴킷, 라이브러리, 도구를 사용합니다.

<br>




### 7.1. DirectX 12 Agility SDK
- 목적
  - 윈도우 업데이트와 상관없이 최신 D3D12 헤더, 런타임 사용

- 사용 방식
  - NuGet 패키지 관리 (Microsoft.Direct3D.D3D12)
  
- 복원 위치
  - `PodoNatureEngine\packages\Microsoft.Direct3D.D3D12.<버전명>`

- 라이선스 종류
  - MICROSOFT SOFTWARE LICENSE
  - MIT License

- 라이선스 위치
  - `PodoNatureEngine\packages\Microsoft.Direct3D.D3D12.<버전명>\LICENSE.txt`
  - `PodoNatureEngine\packages\Microsoft.Direct3D.D3D12.<버전명>\LICENSE-CODE.txt`

<br>




### 7.2. DirectX Tool Kit (DirectXTK12)
- 목적
  - Direct3D 12 유틸리티 라이브러리 사용

- 사용 방식
  - NuGet 패키지 관리 (directxtk12_desktop_win10)

- 복원 위치
  - `PodoNatureEngine\packages\directxtk12_desktop_win10.<버전명>`

- 라이선스 종류
  - MIT License

- 라이선스 위치
  - `PodoNatureEngine\packages\directxtk12_desktop_win10.<버전명>\docs\README.md` 내부 간접 링크
  - `https://github.com/microsoft/DirectXTK12/blob/main/LICENSE`

<br>




### 7.3. PIX Event Runtime
- 목적
  - PIX 캡처

- 사용 방식
  - NuGet 패키지 관리 (WinPixEventRuntime)
  
- 복원 위치
  - `PodoNatureEngine\packages\WinPixEventRuntime.<버전명>`

- 라이선스 종류
  - MIT License

- 라이선스 위치
  - `PodoNatureEngine\packages\WinPixEventRuntime.<버전명>\license.txt`

<br>




### 7.4 Dear ImGui
- 목적
  - 옵션 GUI 제공

- 사용 방식
  - 리포지토리 내부에 직접 포함함(`PodoNatureEngine\External\imgui`)
  - 예제 코드의 일부를 수정하여 사용함(`PodoNatureEngine\Code\Header\Alloc.h`)

- 원본 리포지토리
  - ocornut/imgui  
    `https://github.com/ocornut/imgui`

- 라이선스 종류
  - MIT License

- 라이선스 위치
  - `PodoNatureEngine\External\imgui\LICENSE.txt`

<br>




<!---------------------------------------------------------------------------------------------------------------------------------------------->
## 8. 구현 참고
"Introduction To 3D Game Programming With DirectX 12 Second Edition" (Frank D. Luna 지음 / Mercury Learning And Information 출판 / 2025년 발행) 

"Get Started with Win32 and C++" (Microsoft Learn / 2026년 6월 열람)  
[https://learn.microsoft.com/en-us/windows/win32/learnwin32/learn-to-program-for-windows](https://learn.microsoft.com/en-us/windows/win32/learnwin32/learn-to-program-for-windows)

"Link an executable to a DLL" (Microsoft Learn / 2026년 7월 열람)  
[https://learn.microsoft.com/en-us/cpp/build/linking-an-executable-to-a-dll?view=msvc-170](https://learn.microsoft.com/en-us/cpp/build/linking-an-executable-to-a-dll?view=msvc-170)

"Getting Started with the Agility SDK" (Microsoft Dev Blogs / 2026년 7월 열람)  
[https://devblogs.microsoft.com/directx/gettingstarted-dx12agility](https://devblogs.microsoft.com/directx/gettingstarted-dx12agility)

"Setting the default DPI awareness for a process" (Microsoft Learn / 2026년 7월 열람)  
[https://learn.microsoft.com/en-us/windows/win32/hidpi/setting-the-default-dpi-awareness-for-a-process](https://learn.microsoft.com/en-us/windows/win32/hidpi/setting-the-default-dpi-awareness-for-a-process)

"ComPtr" (Microsoft / 2026년 7월 열람)  
[https://github.com/Microsoft/DirectXTK/wiki/ComPtr](https://github.com/Microsoft/DirectXTK/wiki/ComPtr)

"The Care and Feeding of Modern Swap Chains" (Chuck Walbourn / 2026년 7월 열람)  
[https://walbourn.github.io/care-and-feeding-of-modern-swapchains](https://walbourn.github.io/care-and-feeding-of-modern-swapchains)  
[https://walbourn.github.io/care-and-feeding-of-modern-swap-chains-2](https://walbourn.github.io/care-and-feeding-of-modern-swap-chains-2)  
[https://walbourn.github.io/care-and-feeding-of-modern-swap-chains-3](https://walbourn.github.io/care-and-feeding-of-modern-swap-chains-3)

"PresentMon" (GameTechDev / 2026년 7월 열람)  
[https://github.com/GameTechDev/PresentMon](https://github.com/GameTechDev/PresentMon)

"WinPixEventRuntime" (Microsoft Dev Blogs / 2026년 7월 열람)  
[https://devblogs.microsoft.com/pix/winpixeventruntime](https://devblogs.microsoft.com/pix/winpixeventruntime)

<br>