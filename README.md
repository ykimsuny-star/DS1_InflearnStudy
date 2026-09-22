# DS1 · Inflearn Study

인프런 소울라이크 게임 개발 강의를 따라 Unreal Engine과 C++를 학습하는 실습 프로젝트입니다. 다크 소울, 엘든 링과 같은 소울류 게임의 캐릭터 조작과 게임 시스템 구현을 학습합니다.

- 강의: [실전! 언리얼 소울라이크 개발: A to Z](https://www.inflearn.com/course/언리얼-소울라이크-개발)
- 저장소: [DS1_InflearnStudy](https://github.com/ykimsuny-star/DS1_InflearnStudy)
- 기본 브랜치: `main`
- 강의 엔진: Unreal Engine **5.5**
- 실습 엔진: Unreal Engine **5.8.2** (`EngineAssociation`: `5.8`)
- 개발 언어: C++ / Blueprint

## 개발 환경

- Windows 64비트
- Unreal Engine 5.8.2
- Visual Studio 2022의 C++ 게임 개발 도구와 Windows SDK
- Git 및 Git LFS

검증에 사용한 환경은 MSVC 14.44 및 Windows SDK 10.0.22621.0입니다.

## 내려받기와 실행

```powershell
git lfs install
git clone https://github.com/ykimsuny-star/DS1_InflearnStudy.git
cd DS1_InflearnStudy
git lfs pull
```

1. Unreal Engine 5.8.2를 설치합니다.
2. `DS1.uproject`를 우클릭하여 **Generate Visual Studio project files**를 실행합니다.
3. 생성된 솔루션에서 **Development Editor / Win64** 구성으로 빌드합니다.
4. `DS1.uproject`를 열고 기본 맵 `/Game/_Game/Maps/DevMap`에서 플레이합니다.

엔진을 기본 위치에 설치했다면 다음 명령으로도 에디터 모듈을 빌드할 수 있습니다.

```powershell
& 'C:/Program Files/Epic Games/UE_5.8/Engine/Build/BatchFiles/Build.bat' DS1Editor Win64 Development "-Project=$PWD/DS1.uproject" -WaitMutex -NoHotReloadFromIDE
```

`.uasset`과 `.umap`은 Git LFS로 관리합니다. ZIP 다운로드보다는 위의 Git 복제 방법을 사용하세요.

## 프로젝트 구성

| 경로 | 내용 |
| --- | --- |
| `Source/DS1/Characters` | 캐릭터와 입력 처리 |
| `Source/DS1/Animation` | 애니메이션 인스턴스 |
| `Source/DS1/Components` | 속성 및 상태 컴포넌트 |
| `Source/DS1/UI` | HUD와 스탯 바 |
| `Source/DS1/GameModes`, `Player` | 게임 모드와 플레이어 컨트롤러 |
| `Config` | 프로젝트 기본 설정 |
| `Content` | 블루프린트, 맵, 애니메이션 및 기타 에셋 |

## 버전 관리

`Source`, `Config`, `Content`, `.uproject`, 필요한 `Build` 리소스를 추적합니다. `Content/__ExternalActors__`와 `Content/__ExternalObjects__`는 맵 구성에 필요한 원본 에셋이므로 포함합니다.

다음 항목은 로컬에서 재생성되므로 제외합니다.

- `Binaries`, `DerivedDataCache`, `Intermediate`, `Saved`
- Visual Studio 및 IDE 개인 설정과 생성된 솔루션
- `Content/Developers`, `Content/Collections`의 로컬 작업 자료

강의 진행 단위로 `main`에 변경을 기록하고, 커밋 전에 `git status`로 포함 파일을 확인합니다. Git LFS가 활성화된 상태에서 에셋을 추가해야 합니다.

## 5.5 → 5.8 전환 기록

- 프로젝트 엔진 연결을 `5.8`로 변경했습니다.
- Game/Editor 빌드 타깃에 `BuildSettingsVersion.V7`과 `EngineIncludeOrderVersion.Unreal5_8`을 적용했습니다.
- `UUserWidget` 기반 공개 헤더가 사용하는 `UMG` 모듈을 명시적으로 추가했습니다.
- 2026-09-23: Unreal Engine 5.8.2에서 `DS1Editor / Win64 / Development` C++ 빌드 성공.
- 2026-09-23: `CompileAllBlueprints` 검사 성공 — 컴파일 오류 0건, 경고 0건, 로드 실패 0건.

에셋 일괄 재저장은 수행하지 않았습니다. 에디터에서 수정하고 저장하는 에셋부터 5.8 형식으로 저장됩니다. 플레이 동작은 에디터에서 별도로 확인해야 합니다.

## 출처

학습 기반 강의와 포함 에셋의 출처 및 원래 라이선스는 각각 유지됩니다. 이 저장소는 공식 강의 배포본이나 FromSoftware의 공식 프로젝트가 아닙니다. 공개 열람 여부와 개별 자료의 재사용 조건은 별개이며, 재사용 시 해당 자료의 이용 조건을 확인하세요.
