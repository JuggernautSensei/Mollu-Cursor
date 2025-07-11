# Hello, Mollu Cursor!

https://github.com/user-attachments/assets/3a5e7ce3-d7ef-4c15-8030-d6ffb6e6f3a0

해당 프로그램은 단순하게 마우스를 원하는 위치로 옮기고 클릭 이벤트를 발생시키는 것이 전부입니다.
다른 응용프로그램에 대한 어떠한 해킹도 없습니다.

해당 프로그램의 특징은 다음과 같습니다.

* 실행파일 용량 1MB 미만, C++ 기반의 가벼운 프로그램
* 사용자 친화적인 GUI
* 어떠한 해킹도 없이 단순 윈도우 메시지 생성 기반
* 클릭, 연타, 홀드, 이동 등 다양한 매크로 액션
* 프로그램 자동 감지, 더하여 블루아카이브 뿐만 아니라 다른 프로그램에서도 사용 가능
* 프로그램 위치, 크기에 관계없이 일관적인 좌표 이동 제공
* 매크로 저장 및 불러오기
* 매크로 시각화 및 오버레이

## 다운로드
[MolluCursor.zip](https://github.com/user-attachments/files/21177376/MolluCursor.zip)

### 구성파일
* fonts: 프로그램에 사용되는 폰트 파일입니다. 해당 파일이 손상되면 실행되지 않습니다.
* MolluCursor_x64.exe: x64 용 실행파일입니다. 일반적으로 이걸 실행시키면 됩니다.
* MolluCursor_Win32.exe: x86 용 실행파일입니다.

<details>
<summary> Windows PC 보호 경고 메시지가 뜰 경우 </summary>

<img width="776" height="735" alt="image" src="https://github.com/user-attachments/assets/def2f333-b82e-4b06-ad9e-13b141a0be6a" />


해당 경고가 뜨는 이유는 인증서가 없기 때문입니다.
무료 프로그램에다가 소스코드도 다 공개되어있는데 돈주고 인증서까지 적용시킬 여유가 없습니다.


<img width="778" height="731" alt="image" src="https://github.com/user-attachments/assets/e1cb4478-3494-46ce-a1ce-04e0a142fe1a" />

<img width="783" height="730" alt="image" src="https://github.com/user-attachments/assets/07dba985-0150-4615-805c-f3472ea05ba3" />


위와 같은 방법으로 프로그램을 실행시킬 수 있습니다.
코드를 읽으실 수 있으면 아시겠지만 특별히 악성 코드나 해킹, 후킹 등의 코드는 존재하지 않습니다.  
배포용 exe 파일에도 장난쳐놓지 않았구요.

영 불안하시면 소스파일 직접 빌드해서 사용하시면 되겠습니다.

</details>

## 개발 환경 및 요구사항
* 개발 언어: C++ 20
* 컴파일러: Microsoft Visual C++ (MSVC) 
* 백엔드: Windows API, DirectX 11

Windows 10, 11 (x86, x64) 에서 작동 확인  
Windows가 아닌 운영체제에서는 실행 불가능

## 사용법

### 간단한 사용 영상
https://github.com/user-attachments/assets/3b8819a2-7852-4ec6-bb2f-31dfe609308d

### 다양한 마우스 입력

https://github.com/user-attachments/assets/941a226c-e30f-40bd-b7dd-450187661d5c

### 매크로 저장 및 불러오기

https://github.com/user-attachments/assets/e0df0a44-5182-4230-b32b-dfa103b962cd

### 다른 프로그램에 사용하는 방법

https://github.com/user-attachments/assets/1e2c2eb6-46ec-4d1f-86a1-b9f3b2efc58c

'감지 풀기' 버튼을 클릭한 후 감지하고 싶은 프로세스의 이름을 적으면 됩니다.

## 버그 제보
발생할 수 있는 버그는 다음과 같습니다.

1. 경고 창이 뜨며 오류 메시지가 발생함.
이 경우 자동 생성된 버그 리포트 파일을 그대로 깃허브 Issues 창에 업로드해주시면 되겠습니다.

2. 프로그램이 갑자기 꺼짐
언제 어떻게 무엇을 하다가 버그가 발생하였는지, 버그를 재현할 수 있는 상황을 알려주시면 버그를 수정하는데 도움이 됩니다.


