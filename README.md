# OrbitStick

## 概要
SpaceMouseをエミュレートする自作デバイス（BLE版）。
Fusion360での利用を前提に作られているが、他のソフトウェアでも利用可能。

## 特徴
### BLE接続でワイヤレス動作
Bluetooth Low Energy (BLE)を使用し、ワイヤレスで動作します。
SpaceMouseとしてBLE HIDデバイスで動作するため、マウスとの併用が可能。

### ジョイスティック2本搭載
一方のジョイスティックは上下左右の視点移動（Pan）専用。
他方はオブジェクト回転操作（Orbit）専用。

## Feature
- Pan
- Orbit
- Zoom
- TopView, RightView, FrontViewの切り替え
- Viewの回転
- カスタムショートカットキー（F4: 画面にフィットなど）
- BLE接続によるワイヤレス動作

## ハードウェア構成
- XIAO nRF52840 (Bluetooth LE搭載)
- Joystick *2
- push button

## 実装
BLE HIDプロトコルを使用して3Dconnexion SpaceMouseとして動作します。
BLE HIDレポートを送信することで、ホストデバイスに3Dマウスとして認識されます。

## セットアップ
1. PlatformIOを使用してプロジェクトをビルド
2. XIAO nRF52840にプログラムを書き込み
3. デバイスのBluetooth設定から"OrbitStick SpaceMouse"を検索してペアリング
4. Fusion360などの3D CADソフトウェアで使用

## 参考
https://github.com/FaqT0tum/Orbion_3D_Space_Mouse
https://github.com/lukenuttall/SpaceNavigatorEmulator