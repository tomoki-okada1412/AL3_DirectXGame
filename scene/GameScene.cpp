#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include<random>

using namespace DirectX;

GameScene::GameScene() {
	//ワールドトランスフォームの初期化
	//worldTransform_.Initialize();
	//ビュープロダクションの初期化
	//viewProjection_.Initialize();
}

GameScene::~GameScene() {
	delete sprite_;
	delete model_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	//ファイル名を指定してテクスチャを読み込む(2D画像)
	// textureHandle2D_ = TextureManager::Load("mario.jpg");
	textureHandle3D_ = TextureManager::Load("mario.jpg");
	//サウンドデータの読み込み
	// soundDataHandle_ = audio_->LoadWave("se_sad03.wav");

	//音声再生
	// audio_->PlayWave(soundDataHandle_);
	//音声再生(ループ)
	// audio_->PlayWave(soundDataHandle_,true);
	//音量調整
	// audio_->SetVolume(soundDataHandle_, 0.01f);

	//乱数シード生成器
	std::random_device seed_gen;
	//メルセンヌ・ツイスター(今(2022年4月19日)の僕は誰だお前状態)
	std::mt19937_64 engine(seed_gen());
	//乱数範囲(回転角用)
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);
	//乱数範囲(座標用)
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

	//  生成(2D画像)
	sprite_ = Sprite::Create(textureHandle2D_, {100, 50});

	// 3Dモデルの生成
	model_ = Model::Create();

	for (size_t i = 0; i < _countof(worldTransform_); i++) {

		// X, Y, Z方向のスケーリングを設定
		worldTransform_[i].scale_ = {1.0f, 1.0f, 1.0f};

		// X, Y, Z軸周りの回転角を設定
		worldTransform_[i].rotation_ = {rotDist(engine), rotDist(engine), rotDist(engine)};
		// XM_PIはπのことで180°になる。単位はラジアン
		// XMConvertToRadiansを使用することで度数法に変換可能

		// X, Y, Z軸周りの平行移動を設定
		worldTransform_[i].translation_ = {posDist(engine), posDist(engine), posDist(engine)};

		//ワールドトランスフォームの初期化
		worldTransform_[i].Initialize();
	}
	//カメラ視点座標を設定
	viewProjection_.eye = {0.0f, 0.0f, -50.0f};
	//カメラ注視点座標を設定
	viewProjection_.target = {10.0f, 0.0f, 0.0f};
	//カメラ上方向ベクトルを設定(右上45°指定)
	viewProjection_.up = {cosf(XM_PI / 4.0f), sinf(XM_PI / 4.0f), 0.0f};
	//ビュープロダクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {
	//画像の移動
	//スプライトの今の座標を取得
	XMFLOAT2 position = sprite_->GetPosition();
	//座標を{2,0}移動
	position.x += 2.0f;
	position.y += 1.0f;
	//移動した座標をスプライトに反映
	sprite_->SetPosition(position);

	//視点移動処理
	XMFLOAT3 move = {0.0f, 0.0f, 0.0f};

	//上方向の回転速さ
	const float kUpRotSpeed = 0.05f;

	//視点の移動速さ
	const float kTargetSpeed = 0.2f;
	const float kEyeSpeed = 0.2f;

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_W)) {

		move = {0.0f, 0.0f, kEyeSpeed};
	} 
	else if (input_->PushKey(DIK_S)) {
		move = {0.0f, 0.0f, -kEyeSpeed};
	} 
	else if (input_->PushKey(DIK_LEFT)) {
		 move = {kTargetSpeed, 0.0f, 0.0f};
	} 
	else if (input_->PushKey(DIK_RIGHT)) {
		move = {-kTargetSpeed, 0.0f, 0.0f};
	} 
	else if (input_->PushKey(DIK_SPACE)) {
		viewAngle -= kUpRotSpeed;
		//2π越えたら戻す
		viewAngle = fmodf(viewAngle, XM_2PI);
	}
	//視点移動(ベクトルの加算)
	viewProjection_.target.x += move.x;
	viewProjection_.eye.z += move.z;

	//上方向ベクトルを計算
	viewProjection_.up = {cosf(viewAngle), sinf(viewAngle), 0.0f};

	//行列の再計算
	viewProjection_.UpdateMatrix();

	//デバッグ表示用
	debugText_->SetPos(50, 50);
	debugText_->Printf(
	  "eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, 
		viewProjection_.eye.z);

	debugText_->SetPos(50, 70);
	debugText_->Printf(
	  "target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y, 
		viewProjection_.target.z);

	debugText_->SetPos(50, 90);
	debugText_->Printf(
	  "up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, 
		viewProjection_.up.z);

	//変数の値をインクリメント
	//value_++;
	//値を含んだ文字列
	//std::string strDebug = std::string("translation:") + 
	//std::to_string();
	//デバッグテキストを表示
	//debugText_->Print(strDebug, 50, 50, 1.0f);

	/*debugText_->SetPos(50, 50);
	debugText_->Printf(
	  "translation:(%f,%f,%f)", worldTransform_.translation_.x, worldTransform_.translation_.y,
	  worldTransform_.translation_.z);
	
	debugText_->SetPos(50, 70);
	debugText_->Printf(
	  "rotation:(%f,%f,%f)", worldTransform_.rotation_.x, worldTransform_.rotation_.y,
	  worldTransform_.rotation_.z);

	debugText_->SetPos(50, 90);
	debugText_->Printf(
	  "scale:(%f,%f,%f)", worldTransform_.scale_.x, worldTransform_.scale_.y,
	  worldTransform_.scale_.z);*/
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
	//sprite_->Draw(); 
	//↑ここに書くと、2Dモデルが奥に表示される

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
	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		model_->Draw(worldTransform_[i], viewProjection_, textureHandle3D_);
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
	sprite_->Draw();
	//↑ここに書くことで3Dモデルの方が奥に行く

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
