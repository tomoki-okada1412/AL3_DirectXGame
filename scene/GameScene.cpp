#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

using namespace DirectX;

GameScene::GameScene() {
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	//ビュープロダクションの初期化
	viewProjection_.Initialize();
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
	textureHandle2D_ = TextureManager::Load("mario.jpg");
	textureHandle3D_ = TextureManager::Load("mario.jpg");
	//サウンドデータの読み込み
	soundDataHandle_ = audio_->LoadWave("se_sad03.wav");

	//音声再生
	//audio_->PlayWave(soundDataHandle_);
	//音声再生(ループ)
	audio_->PlayWave(soundDataHandle_,true);
	//音量調整
	audio_->SetVolume(soundDataHandle_, 0.01f);

	//  生成(2D画像)
	sprite_ = Sprite::Create(textureHandle2D_, {100,50});

	//3Dモデルの生成
	model_ = Model::Create();
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

	//スペースキーを推した瞬間
	if (input_->TriggerKey(DIK_SPACE)) {
		//音声を止めます
		audio_->StopWave(voiceHandle_);
	}
	//変数の値をインクリメント
	value_++;
	//値を含んだ文字列
	std::string strDebug = std::string("value:") + 
	std::to_string(value_);
	//デバッグテキストを表示
	debugText_->Print(strDebug, 50, 50, 1.0f);
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
	//sprite_->Draw(); ←ここに書くと、2Dモデルが奥に表示される

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
	model_->Draw(worldTransform_, viewProjection_, textureHandle3D_);

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
