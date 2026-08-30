// ============================================================================
// パラメータファイルの読み書き
// ============================================================================
// JSON と YAML のどちらで書いても、読み戻したときに同じ値へ戻ることを見る。
//
// YAML は書き方に幅があり、"42" や "true" のような文字列を引用符なしで
// 書くと数や真偽として読まれてしまう。書き出す側で文字列を必ず引用符で
// 囲み、読む側は引用符の有無で見分けている。そこが崩れていないかを
// 確かめるのがこのテストの主目的。

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest/doctest.h"

#include <JuceHeader.h>

#include "Core/Io/ParamFile.h"

namespace
{
	const Io::ParamFormat testFormat{ "paramFileTest", 1 };

	// 使い終わったら消える置き場
	struct TempFile
	{
		juce::File file;

		TempFile() : file(juce::File::createTempFile(".paramtest")) {}
		~TempFile() { file.deleteFile(); }
	};

	// 書き出す形を元へ戻してから抜ける。どのテストから走っても同じ結果に
	// なるようにするため。
	struct ScopedFormat
	{
		Io::FileFormat previous;

		explicit ScopedFormat(Io::FileFormat format) : previous(Io::getFileFormat())
		{
			Io::setFileFormat(format);
		}

		~ScopedFormat() { Io::setFileFormat(previous); }
	};
}

TEST_CASE("パラメータファイル: 素の値がどちらの形でも往復する")
{
	for (auto format : { Io::FileFormat::json, Io::FileFormat::yaml })
	{
		ScopedFormat scoped(format);
		TempFile temp;

		{
			Io::ParamWriter writer(testFormat);

			writer.set("count", 42);
			writer.set("ratio", 0.5f);
			writer.set("enabled", true);
			writer.set("disabled", false);
			writer.set("name", juce::String("test"));

			REQUIRE(writer.writeTo(temp.file));
		}

		auto reader = Io::ParamReader::open(temp.file, testFormat, false);

		REQUIRE(reader.has_value());
		CHECK(reader->getInt("count") == 42);
		CHECK(reader->getFloat("ratio") == doctest::Approx(0.5f));
		CHECK(reader->getBool("enabled") == true);
		CHECK(reader->getBool("disabled") == false);
		CHECK(reader->getString("name") == "test");

		// 書かれていない名前は既定値のまま
		CHECK(reader->getInt("missing", 7) == 7);
	}
}

TEST_CASE("パラメータファイル: 数に見える文字列が文字列のまま戻る")
{
	// ここが崩れると、ファイル名やコメントが読み込みのたびに書き換わる
	for (auto format : { Io::FileFormat::json, Io::FileFormat::yaml })
	{
		ScopedFormat scoped(format);
		TempFile temp;

		{
			Io::ParamWriter writer(testFormat);

			writer.set("digits", juce::String("42"));
			writer.set("leadingZero", juce::String("0100"));
			writer.set("looksBool", juce::String("true"));
			writer.set("looksNull", juce::String("null"));
			writer.set("decimal", juce::String("1.50"));
			writer.set("path", juce::String("C:\\Sounds\\01.wav"));
			writer.set("japanese", juce::String::fromUTF8(u8"和音"));

			REQUIRE(writer.writeTo(temp.file));
		}

		auto reader = Io::ParamReader::open(temp.file, testFormat, false);

		REQUIRE(reader.has_value());
		CHECK(reader->getString("digits") == "42");
		CHECK(reader->getString("leadingZero") == "0100");
		CHECK(reader->getString("looksBool") == "true");
		CHECK(reader->getString("looksNull") == "null");
		CHECK(reader->getString("decimal") == "1.50");
		CHECK(reader->getString("path") == "C:\\Sounds\\01.wav");
		CHECK(reader->getString("japanese") == juce::String::fromUTF8(u8"和音"));
	}
}

TEST_CASE("パラメータファイル: 小数がそのままの値で戻る")
{
	// float から double へ広げた値をそのまま書き出しているので、
	// 書き方を間違えると音の設定がわずかにずれる
	const float values[] = { 0.1f, 1.0f / 3.0f, 0.0f, -12.75f, 1.0e-6f };

	for (auto format : { Io::FileFormat::json, Io::FileFormat::yaml })
	{
		ScopedFormat scoped(format);
		TempFile temp;

		{
			Io::ParamWriter writer(testFormat);

			for (int i = 0; i < (int)std::size(values); ++i)
			{
				writer.set("v" + juce::String(i), values[i]);
			}

			REQUIRE(writer.writeTo(temp.file));
		}

		auto reader = Io::ParamReader::open(temp.file, testFormat, false);

		REQUIRE(reader.has_value());

		for (int i = 0; i < (int)std::size(values); ++i)
		{
			CHECK(reader->getFloat("v" + juce::String(i)) == values[i]);
		}
	}
}

TEST_CASE("パラメータファイル: 入れ子と並びが往復する")
{
	for (auto format : { Io::FileFormat::json, Io::FileFormat::yaml })
	{
		ScopedFormat scoped(format);
		TempFile temp;

		{
			Io::ParamWriter writer(testFormat);

			auto lfo = writer.child("lfo");

			lfo.set("speed", 3);

			for (int i = 0; i < 4; ++i)
			{
				auto op = writer.arrayItem(Io::ParamKey::ops, i);

				op.set("tl", i * 10);
				op.set("name", juce::String("op") + juce::String(i));
			}

			writer.setArray("wave", std::vector<float>{ 0.0f, 0.25f, -1.0f });
			writer.setArray("steps", std::vector<int>{ 1, 2, 3 });

			REQUIRE(writer.writeTo(temp.file));
		}

		auto reader = Io::ParamReader::open(temp.file, testFormat, false);

		REQUIRE(reader.has_value());
		CHECK(reader->child("lfo").getInt("speed") == 3);
		CHECK(reader->arraySize(Io::ParamKey::ops) == 4);

		for (int i = 0; i < 4; ++i)
		{
			auto op = reader->arrayItem(Io::ParamKey::ops, i);

			CHECK(op.getInt("tl") == i * 10);
			CHECK(op.getString("name") == juce::String("op") + juce::String(i));
		}

		auto wave = reader->getFloatArray("wave");

		REQUIRE(wave.size() == 3);
		CHECK(wave[1] == doctest::Approx(0.25f));

		auto steps = reader->getIntArray("steps");

		REQUIRE(steps.size() == 3);
		CHECK(steps[2] == 3);
	}
}

TEST_CASE("パラメータファイル: 書いた形と違う形でも読める")
{
	// 読み込みは中身を見て振り分けているので、設定を切り替えた後でも
	// 前に書き出したファイルがそのまま読める
	TempFile temp;

	{
		ScopedFormat scoped(Io::FileFormat::yaml);

		Io::ParamWriter writer(testFormat);

		writer.set("count", 5);
		writer.set("name", juce::String("yaml"));

		REQUIRE(writer.writeTo(temp.file));
	}

	{
		ScopedFormat scoped(Io::FileFormat::json);

		auto reader = Io::ParamReader::open(temp.file, testFormat, false);

		REQUIRE(reader.has_value());
		CHECK(reader->getInt("count") == 5);
		CHECK(reader->getString("name") == "yaml");
	}
}

TEST_CASE("パラメータファイル: 手で書いた YAML も読める")
{
	// 引用符を省いた書き方や、YAML でよく使う真偽の書き方を受ける
	TempFile temp;

	temp.file.replaceWithText(
		"format: paramFileTest\n"
		"version: 1\n"
		"values:\n"
		"  count: 12\n"
		"  ratio: 0.25\n"
		"  enabled: yes\n"
		"  disabled: off\n"
		"  name: hello\n"
		"  ops:\n"
		"    - tl: 1\n"
		"    - tl: 2\n");

	auto reader = Io::ParamReader::open(temp.file, testFormat, false);

	REQUIRE(reader.has_value());
	CHECK(reader->getInt("count") == 12);
	CHECK(reader->getFloat("ratio") == doctest::Approx(0.25f));
	CHECK(reader->getBool("enabled") == true);
	CHECK(reader->getBool("disabled") == false);
	CHECK(reader->getString("name") == "hello");
	CHECK(reader->arraySize("ops") == 2);
	CHECK(reader->arrayItem("ops", 1).getInt("tl") == 2);
}

TEST_CASE("パラメータファイル: 別の種類や古い形式は開かない")
{
	SUBCASE("印が違う")
	{
		TempFile temp;

		{
			Io::ParamWriter writer({ "otherKind", 1 });

			writer.set("count", 1);

			REQUIRE(writer.writeTo(temp.file));
		}

		CHECK_FALSE(Io::ParamReader::open(temp.file, testFormat, false).has_value());
	}

	SUBCASE("3.0.0 より前の並びだけのファイル")
	{
		TempFile temp;

		temp.file.replaceWithText("12\n34\n56\n");

		CHECK_FALSE(Io::ParamReader::open(temp.file, testFormat, false).has_value());
	}

	SUBCASE("そもそも無い")
	{
		juce::File missing = juce::File::getSpecialLocation(juce::File::tempDirectory)
			.getChildFile("no_such_param_file.json");

		CHECK_FALSE(Io::ParamReader::open(missing, testFormat, false).has_value());
	}
}

TEST_CASE("パラメータファイル: 拡張子が選んだ形に付く")
{
	{
		ScopedFormat scoped(Io::FileFormat::json);

		CHECK(Io::fileFormatExtension() == "json");
		CHECK(Io::defaultFileName("param.opna") == "default.param.opna.json");
		CHECK(Io::saveGlob("param.opna") == "*.param.opna.json");
	}

	{
		ScopedFormat scoped(Io::FileFormat::yaml);

		CHECK(Io::fileFormatExtension() == "yaml");
		CHECK(Io::defaultFileName("param.opna") == "default.param.opna.yaml");
		CHECK(Io::saveGlob("param.opna") == "*.param.opna.yaml");
	}

	// 読み込みはどちらの形も選べる
	CHECK(Io::openGlob("param.opna") == "*.param.opna.json;*.param.opna.yaml");
}

TEST_CASE("パラメータファイル: プリセットの中身が往復する")
{
	// APVTS の状態は XML で組み立てているので、そこを通した往復も見る
	for (auto format : { Io::FileFormat::json, Io::FileFormat::yaml })
	{
		ScopedFormat scoped(format);
		TempFile temp;

		juce::XmlElement source("PARAMS");

		source.setAttribute("presetName", "Bass 01");
		source.setAttribute("presetAuthor", "2686");

		for (int i = 0; i < 3; ++i)
		{
			auto* param = source.createNewChildElement("PARAM");

			param->setAttribute("id", "op" + juce::String(i) + "_tl");
			param->setAttribute("value", juce::String(i) + ".5");
		}

		auto* curve = source.createNewChildElement("CURVE");

		curve->setAttribute("target", "amp");

		auto* point = curve->createNewChildElement("POINT");

		point->setAttribute("x", "0.25");

		{
			Io::ParamWriter writer(testFormat);

			Io::writeStateXml(writer, source);

			REQUIRE(writer.writeTo(temp.file));
		}

		auto reader = Io::ParamReader::open(temp.file, testFormat, false);

		REQUIRE(reader.has_value());

		auto restored = Io::readStateXml(*reader, "PARAMS");

		REQUIRE(restored != nullptr);
		CHECK(restored->getStringAttribute("presetName") == "Bass 01");
		CHECK(restored->getStringAttribute("presetAuthor") == "2686");

		for (int i = 0; i < 3; ++i)
		{
			bool found = false;

			for (auto* param : restored->getChildWithTagNameIterator("PARAM"))
			{
				if (param->getStringAttribute("id") != "op" + juce::String(i) + "_tl") continue;

				found = true;

				CHECK(param->getStringAttribute("value") == juce::String(i) + ".5");
			}

			CHECK(found);
		}

		auto* restoredCurve = restored->getChildByName("CURVE");

		REQUIRE(restoredCurve != nullptr);
		CHECK(restoredCurve->getStringAttribute("target") == "amp");

		auto* restoredPoint = restoredCurve->getChildByName("POINT");

		REQUIRE(restoredPoint != nullptr);
		CHECK(restoredPoint->getStringAttribute("x") == "0.25");
	}
}

TEST_CASE("パラメータファイル: 独自の形の中身も選んだ形で書ける")
{
	// 色の設定のように ParamWriter を通さないものが使う入口
	for (auto format : { Io::FileFormat::json, Io::FileFormat::yaml })
	{
		ScopedFormat scoped(format);
		TempFile temp;

		auto* root = new juce::DynamicObject();

		root->setProperty("format", "colors");
		root->setProperty("count", 3);

		REQUIRE(Io::writeValueTo(temp.file, juce::var(root)));

		auto parsed = Io::readValueFrom(temp.file);
		auto* restored = parsed.getDynamicObject();

		REQUIRE(restored != nullptr);
		CHECK(restored->getProperty("format").toString() == "colors");
		CHECK((int)restored->getProperty("count") == 3);
	}
}

TEST_CASE("パラメータファイル: 入れ子を外側の中身へ引き上げる")
{
	// 部品ごとの単体ファイルは、チャンネルのファイルの中に入っている
	// まとまりと中身が同じなので、writeParams をそのまま使えるようにする
	for (auto format : { Io::FileFormat::json, Io::FileFormat::yaml })
	{
		ScopedFormat scoped(format);
		TempFile temp;

		{
			Io::ParamWriter writer(testFormat);

			auto child = writer.child("ampEnv");

			child.set("attack", 0.25f);
			child.set("kor", true);

			writer.hoist("ampEnv");

			REQUIRE(writer.writeTo(temp.file));
		}

		auto reader = Io::ParamReader::open(temp.file, testFormat, false);

		REQUIRE(reader.has_value());

		// 引き上げたので、入れ子ではなく直に読める
		CHECK(reader->getFloat("attack") == doctest::Approx(0.25f));
		CHECK(reader->getBool("kor") == true);
		CHECK(reader->child("ampEnv").getFloat("attack", -1.0f) == doctest::Approx(-1.0f));
	}
}

TEST_CASE("パラメータファイル: 3.0.0 より前のファイルを見分ける")
{
	TempFile temp;

	SUBCASE("数の並びだけのもの")
	{
		temp.file.replaceWithText("1\n0.5\n0.25\n0\n");

		CHECK(Io::isLegacyFile(temp.file));
	}

	SUBCASE("空行から始まっていても見分ける")
	{
		temp.file.replaceWithText("\n\n12\n34\n");

		CHECK(Io::isLegacyFile(temp.file));
	}

	SUBCASE("名前式で読めるものは違う")
	{
		Io::ParamWriter writer(testFormat);

		writer.set("count", 1);

		REQUIRE(writer.writeTo(temp.file));
		CHECK_FALSE(Io::isLegacyFile(temp.file));
	}

	SUBCASE("XML は違う")
	{
		temp.file.replaceWithText("<?xml version=\"1.0\"?>\n<Parameters presetName=\"a\"/>\n");

		CHECK_FALSE(Io::isLegacyFile(temp.file));
	}

	SUBCASE("数で始まらない文字はただの壊れたファイル")
	{
		temp.file.replaceWithText("hello\nworld\n");

		CHECK_FALSE(Io::isLegacyFile(temp.file));
	}

	SUBCASE("空のファイル")
	{
		temp.file.replaceWithText("");

		CHECK_FALSE(Io::isLegacyFile(temp.file));
	}
}

TEST_CASE("パラメータファイル: 変換したものの置き場")
{
	auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory);

	// 元の名前を残したまま拡張子を足す。元のファイルは消さない。
	{
		ScopedFormat scoped(Io::FileFormat::json);

		CHECK(Io::convertedFileFor(dir.getChildFile("bass.ampEnv")).getFileName() == "bass.ampEnv.json");
	}

	{
		ScopedFormat scoped(Io::FileFormat::yaml);

		CHECK(Io::convertedFileFor(dir.getChildFile("bass.ampEnv")).getFileName() == "bass.ampEnv.yaml");
	}
}
