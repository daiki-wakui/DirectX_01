#include "GameScene.h"
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

	//キャラクターの大元(親)
	worldTransfrom_[PartId::Root].Initialize();

	//脊椎
	worldTransfrom_[PartId::Spine].translation_ = {0, 4.5f, 0};
	worldTransfrom_[PartId::Spine].parent_ = &worldTransfrom_[PartId::Root];
	worldTransfrom_[PartId::Spine].Initialize();

	//上半身
	//胸
	worldTransfrom_[PartId::Chest].translation_ = {0, 0, 0};
	worldTransfrom_[PartId::Chest].parent_ = &worldTransfrom_[PartId::Spine];
	worldTransfrom_[PartId::Chest].Initialize();
	//頭
	worldTransfrom_[PartId::Head].translation_ = {0, 4.5f, 0};
	worldTransfrom_[PartId::Head].parent_ = &worldTransfrom_[PartId::Chest];
	worldTransfrom_[PartId::Head].Initialize();
	//左腕
	worldTransfrom_[PartId::ArmL].translation_ = {-4.5f, 0, 0};
	worldTransfrom_[PartId::ArmL].parent_ = &worldTransfrom_[PartId::Chest];
	worldTransfrom_[PartId::ArmL].Initialize();
	//右腕
	worldTransfrom_[PartId::ArmR].translation_ = {4.5f, 0, 0};
	worldTransfrom_[PartId::ArmR].parent_ = &worldTransfrom_[PartId::Chest];
	worldTransfrom_[PartId::ArmR].Initialize();

	//下半身
	//尻
	worldTransfrom_[PartId::Hip].translation_ = {0, -4.5f, 0};
	worldTransfrom_[PartId::Hip].parent_ = &worldTransfrom_[PartId::Spine];
	worldTransfrom_[PartId::Hip].Initialize();
	//左足
	worldTransfrom_[PartId::LegL].translation_ = {-4.5f, -4.5f, 0};
	worldTransfrom_[PartId::LegL].parent_ = &worldTransfrom_[PartId::Hip];
	worldTransfrom_[PartId::LegL].Initialize();
	//右足
	worldTransfrom_[PartId::LegR].translation_ = {4.5f, -4.5f, 0};
	worldTransfrom_[PartId::LegR].parent_ = &worldTransfrom_[PartId::Hip];
	worldTransfrom_[PartId::LegR].Initialize();


	//視点座標
	//viewProjection_.eye = {0, 0, -50};

	//注視点座標
	//viewProjection_.target = {0, 0, 0};

	//視点の角度
	//viewProjection_.up = {0, 0, 0};

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {

	//キャラクター移動処理
	XMFLOAT3 move = {0, 0, 0};

	//キャラクターの移動の速さ
	const float kCharacterSoeed = 0.2f;

	//上半身の回転の速さ[ラジアン/frame]
	const float kCheakRotSpeed = 0.05f;

	//下半身の回転の速さ[ラジアン/frame]
	const float kHipRotSpeed = 0.05f;

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_LEFT)) {
		move = {-kCharacterSoeed, 0, 0};

	} else if (input_->PushKey(DIK_RIGHT)) {
		move = {kCharacterSoeed, 0, 0};
	}

	//上半身の回転処理
	if (input_->PushKey(DIK_U)) {
		worldTransfrom_[PartId::Chest].rotation_.y -= kCheakRotSpeed;
	} else if (input_->PushKey(DIK_I)) {
		worldTransfrom_[PartId::Chest].rotation_.y += kCheakRotSpeed;
	}

	//下半身の回転処理
	if (input_->PushKey(DIK_J)) {
		worldTransfrom_[PartId::Hip].rotation_.y -= kHipRotSpeed;
	} else if (input_->PushKey(DIK_K)) {
		worldTransfrom_[PartId::Hip].rotation_.y += kHipRotSpeed;
	}

	//注視点移動(ベクトルの加算)
	worldTransfrom_[PartId::Root].translation_.x += move.x;
	worldTransfrom_[PartId::Root].translation_.y += move.y;
	worldTransfrom_[PartId::Root].translation_.z += move.z;
	

	worldTransfrom_[PartId::Root].UpdateMatrix();
	worldTransfrom_[PartId::Spine].UpdateMatrix();
	worldTransfrom_[PartId::Chest].UpdateMatrix();
	worldTransfrom_[PartId::Head].UpdateMatrix();
	worldTransfrom_[PartId::ArmL].UpdateMatrix();
	worldTransfrom_[PartId::ArmR].UpdateMatrix();
	worldTransfrom_[PartId::Hip].UpdateMatrix();
	worldTransfrom_[PartId::LegL].UpdateMatrix();
	worldTransfrom_[PartId::LegR].UpdateMatrix();
	
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

	debugText_->SetPos(50, 150);
	debugText_->Printf(
	  "Root:(%f,%f,%f)",
		worldTransfrom_[PartId::Root].translation_.x,
		worldTransfrom_[PartId::Root].translation_.y, 
		worldTransfrom_[PartId::Root].translation_.z);
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
	//model_->Draw(worldTransfrom_[PartId::Root], viewProjection_, textureHandle_);
	//model_->Draw(worldTransfrom_[PartId::Spine], viewProjection_, textureHandle_);
	model_->Draw(worldTransfrom_[PartId::Chest], viewProjection_, textureHandle_);
	model_->Draw(worldTransfrom_[PartId::Head], viewProjection_, textureHandle_);
	model_->Draw(worldTransfrom_[PartId::ArmL], viewProjection_, textureHandle_);
	model_->Draw(worldTransfrom_[PartId::ArmR], viewProjection_, textureHandle_);
	model_->Draw(worldTransfrom_[PartId::Hip], viewProjection_, textureHandle_);
	model_->Draw(worldTransfrom_[PartId::LegL], viewProjection_, textureHandle_);
	model_->Draw(worldTransfrom_[PartId::LegR], viewProjection_, textureHandle_);

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