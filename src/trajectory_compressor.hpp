﻿#ifndef DIGITALCURLING3_SERVER_TRAJECTORY_COMPRESSOR_HPP
#define DIGITALCURLING3_SERVER_TRAJECTORY_COMPRESSOR_HPP

#include <list>
#include "digitalcurling3/digitalcurling3.hpp"

namespace digitalcurling3_server {

class TrajectoryCompressor {
public:
    struct Difference {
        digitalcurling3::Team team = digitalcurling3::Team::k0;
        size_t index = 0;
        std::optional<digitalcurling3::Transform> value;
        Difference() = default;
        Difference(digitalcurling3::Team team, size_t index, std::optional<digitalcurling3::Transform> const& value)
            : team(team), index(index), value(value) {}
    };

    struct Result {
        float seconds_per_frame;
        digitalcurling3::GameState::Stones start;
        digitalcurling3::GameState::Stones finish;
        std::list<std::vector<Difference>> frames;
        Result();
        void Reset();
    };

    TrajectoryCompressor();

    /// \brief OnStep() を呼び出す前に呼び出す
    ///
    /// \param steps_per_frame 1フレームのステップ数(>= 1)
    /// \param end 試合の現在のエンド
    void Begin(size_t steps_per_frame, std::uint8_t end);

    /// \brief ステップを記録する
    ///
    /// \note 必ず Begin() と End() の間で呼び出す
    void OnStep(digitalcurling3::ISimulator const& simulator);

    /// \brief OnStep() の呼び出しが終わった後に呼び出す
    ///
    /// 呼出し以降， GetResult() が呼び出せるようになる．
    ///
    /// \param simulator ApplyMove() 終了直後のシミュレータ
    void End(digitalcurling3::ISimulator const& simulator);

    /// \brief 結果を取得する
    ///
    /// \return 圧縮の結果．
    Result const& GetResult() const;

private:
    bool active_;
    size_t frame_count_;
    size_t steps_per_frame_;
    std::uint8_t end_;
    digitalcurling3::GameState::Stones prev_stones_;
    Result result_;

    void SetFirstFrame(digitalcurling3::ISimulator const& simulator);
    void AddFrameDiff(digitalcurling3::ISimulator const& simulator);
};


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    TrajectoryCompressor::Difference,
    team,
    index,
    value 
)

void to_json(nlohmann::json &, TrajectoryCompressor::Result const&);
void from_json(nlohmann::json const&, TrajectoryCompressor::Result &);

} // namespace digitalcurling3_server

#endif // DIGITALCURLING3_SERVER_TRAJECTORY_COMPRESSOR_HPP
