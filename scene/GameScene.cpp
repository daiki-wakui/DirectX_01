#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

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

	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");

	//3Dモデルの生成
	model_ = Model::Create();

	//x,y,z方向のスケーリング
	worldTransfrom_.scale_ = {1.0f, 1.0f, 1.0f};

	//x,y,z軸周りの回転角を設定
	worldTransfrom_.rotation_ = {0.0f, 0.0f, 0.0f};

	//z,y,z軸周りの平行移動を設定
	worldTransfrom_.translation_ = {0.0f, 0.0f, 0.0f};

	//カメラ視点座標を設定
	viewProjection_.eye = {0, 15, -20};

	//カメラ注視点座標を設定
	viewProjection_.target = {0, 0, 0};

	//viewProjection_.up = {0.0f, 0.0f, 0.0f};


	//ワールドトランスフォームの初期化
	worldTransfrom_.Initialize();

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {
	//正面ベクトルの長さ
	XMFLOAT3 length = {0, 0, 1};

	//正面ベクトル
	XMFLOAT3 frontVec = {0, 0, 0};

	//始点
	XMFLOAT3 start = {worldTransfrom_.translation_};

	//終点
	XMFLOAT3 end = {0, 0, 0};

	//オブジェクトの移動ベクトル
	XMFLOAT3 move = {0, 0, 0};

	//オブジェクトの回転ベクトル
	XMFLOAT3 rotaMove = {0, 0, 0};

	//オブジェクトの移動スピード
	const float obSpeed = 0.2f;

	//オブジェクトの回転スピード
	const float rotaSpeed = 0.05f;

	//オブジェクトの前進と後退
	if (input_->PushKey(DIK_UP)) {
		move = {
			sinf(worldTransfrom_.rotation_.y)*obSpeed,
			0,
			cosf(worldTransfrom_.rotation_.y)*obSpeed
		};
	} else if (input_->PushKey(DIK_DOWN)) {
		move = {
			sinf(worldTransfrom_.rotation_.y) * -obSpeed,
			0,
			cosf(worldTransfrom_.rotation_.y) * - obSpeed
		};
	}
	//オブジェクトの回転
	if (input_->PushKey(DIK_RIGHT)) {
		rotaMove = {0, rotaSpeed, 0};
	} else if (input_->PushKey(DIK_LEFT)) {
		rotaMove = {0, -rotaSpeed, 0};
	}

	//ベクトルの加算
	worldTransfrom_.translation_.x += move.x;
	worldTransfrom_.translation_.z += move.z;

	//回転
	worldTransfrom_.rotation_.y += rotaMove.y;

	end.x = start.x + length.x;
	end.y = start.y + length.y;
	end.z = start.z + length.z;

	end.x = sinf(worldTransfrom_.rotation_.y);
	end.z = cosf(worldTransfrom_.rotation_.y);
	
	/*frontVec.x = end.x - start.x;
	frontVec.y = end.y - start.y;
	frontVec.z = end.z - start.z;

	frontVec.x /= length.x;
	frontVec.y /= length.y;
	frontVec.z /= length.z;
	
	start.x += frontVec.x;
	start.y += frontVec.x;
	start.z += frontVec.z;*/

	//再計算
	worldTransfrom_.UpdateMatrix();

	//デバッグ
	debugText_->SetPos(0, 0);
	debugText_->Printf(
	  "translation_:(%f,%f,%f)", 
		worldTransfrom_.translation_.x,
		worldTransfrom_.translation_.y,
		worldTransfrom_.translation_.z);

	debugText_->SetPos(0, 20);
	debugText_->Printf(
	  "rotation_:(%f,%f,%f)",
		worldTransfrom_.rotation_.x,
		worldTransfrom_.rotation_.y,
		worldTransfrom_.rotation_.z);

	debugText_->SetPos(0, 40);
	debugText_->Printf("start:(%f,%f,%f)",start.x,start.y,start.z);

	debugText_->SetPos(0, 60);
	debugText_->Printf("end:(%f,%f,%f)", end.x, end.y, end.z);

	debugText_->SetPos(0, 80);
	debugText_->Printf("frontVec:(%f,%f,%f)", frontVec.x, frontVec.y, frontVec.z);
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
	
	model_->Draw(worldTransfrom_, viewProjection_, textureHandle_);


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