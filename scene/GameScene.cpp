﻿#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete model_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	//乱数シード生成器
	std::random_device seed_gen;
	//メルセンヌ・ツイスター
	std::mt19937_64 engine(seed_gen());
	//乱数範囲(回転角用)
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);
	//乱数範囲(座標用)
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);
	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");

	// 3Dモデルの生成
	model_ = Model::Create();

	for (size_t i = 0; i < _countof(worldTransfrom_); i++) {
		// x,y,z方向のスケーリングを設定
		worldTransfrom_[i].scale_ = {1.0f, 1.0f, 1.0f};

		// x,y,z軸周りの回転角を設定
		worldTransfrom_[i].rotation_ = {rotDist(engine), rotDist(engine), rotDist(engine)};

		// x,y,z軸周りに平行移動を設定
		worldTransfrom_[i].translation_ = {posDist(engine), posDist(engine), posDist(engine)};

		//ワールドトランスフォームの初期化
		worldTransfrom_[i].Initialize();
	}

	//視点座標
	//viewProjection_.eye = {0, 0, -50};

	//注視点座標
	//viewProjection_.target = {0, 0, 0};

	//視点の角度
	//viewProjection_.up = {0, 0, 0};

	//垂直方向視野角
	//viewProjection_.fovAngleY = XMConvertToRadians(10.0f);

	//アスペクト比を設定
	//viewProjection_.aspectRatio = 1.0f;

	//ニアクリップ距離を設定
	viewProjection_.nearZ = 52.0f;

	//ファークリップ距離を設定
	viewProjection_.farZ = 53.0f;

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {

	if (input_->PushKey(DIK_UP)) {
		viewProjection_.nearZ += 0.1f;

	} else if (input_->PushKey(DIK_DOWN)) {
		viewProjection_.nearZ -= 0.1f;
	}

	//視野角変更
	if (input_->PushKey(DIK_W)) {
		viewProjection_.fovAngleY += 0.01f;
		viewProjection_.fovAngleY = min(viewProjection_.fovAngleY, XM_PI);

	} else if (input_->PushKey(DIK_S)) {
		viewProjection_.fovAngleY -= 0.01f;
		viewProjection_.fovAngleY = max(viewProjection_.fovAngleY, 0.01f);
	}
	
	//行列の再計算
	viewProjection_.UpdateMatrix();

	//デバッグ
	debugText_->SetPos(50, 50);
	debugText_->Printf(
	  "eye:(%f,%f,%f)", 
		viewProjection_.eye.x,
		viewProjection_.eye.y,
		viewProjection_.eye.z);

	debugText_->SetPos(50, 70);
	debugText_->Printf(
	  "target:(%f,%f,%f)",
		viewProjection_.target.x,
		viewProjection_.target.y,
		viewProjection_.target.z);

	debugText_->SetPos(50, 90);
	debugText_->Printf(
	  "up:(%f,%f,%f)",
		viewProjection_.up.x,
		viewProjection_.up.y,
		viewProjection_.up.z);

	debugText_->SetPos(50, 110);
	debugText_->Printf(
		"fovAngleY(Degree):%f",
		XMConvertToDegrees(viewProjection_.fovAngleY));

	debugText_->SetPos(50, 130);
	debugText_->Printf("nearZ:%f", viewProjection_.nearZ);
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	/// 
	for (size_t i = 0; i < _countof(worldTransfrom_); i++) {
		model_->Draw(worldTransfrom_[i], viewProjection_, textureHandle_);
	}
	

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>


	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}