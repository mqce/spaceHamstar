# OrbitStick

## 概要
SpaceMouseをエミュレートする自作デバイス。
Fusion360での利用を前提に作られているが、他のソフトウェアでも利用可能。

## 特徴
### マウスと併用可能
SpaceMouseになりすまして動作するため、マウスとの併用が可能。

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

## ハードウェア構成
- Arduino Pro Micro
- Joystick *2
- push button

## 実装
3DconnexionのVIDとPIDを偽装してHIDレポートをFusionに送る。
