// @ts-check
import { defineConfig } from 'astro/config';
import starlight from '@astrojs/starlight';

// GitHub Pages はリポジトリ名の下に置かれるので base が要る。
// https://cyross.github.io/2686V_2686VFX/
// サイドバーの表示・非表示。
//
// 描き直す前に当てないと、閉じている人の画面で一瞬サイドバーが見えてしまう
// ので、head へ入れて先に動かす。押したときの動きも同じところに置き、
// document へ 1 つだけ聞き手を付ける。ボタンはページごとに作り直されるため。
const sidebarToggleScript = `
(function () {
	var root = document.documentElement;

	function mark(hidden) {
		var list = document.querySelectorAll('.sidebar-toggle');

		for (var i = 0; i < list.length; i++) {
			list[i].setAttribute('aria-pressed', String(hidden));
		}
	}

	try {
		if (localStorage.getItem('sidebar') === 'hidden') root.dataset.sidebar = 'hidden';
	} catch (e) {}

	document.addEventListener('click', function (ev) {
		var button = ev.target.closest && ev.target.closest('.sidebar-toggle');

		if (!button) return;

		var hidden = root.dataset.sidebar !== 'hidden';

		if (hidden) {
			root.dataset.sidebar = 'hidden';
		} else {
			delete root.dataset.sidebar;
		}

		mark(hidden);

		try {
			localStorage.setItem('sidebar', hidden ? 'hidden' : 'shown');
		} catch (e) {}
	});

	document.addEventListener('DOMContentLoaded', function () {
		mark(root.dataset.sidebar === 'hidden');
	});
})();
`;

export default defineConfig({
	site: 'https://cyross.github.io',
	base: '/2686V_2686VFX',
	integrations: [
		starlight({
			title: {
				ja: '2686Vファミリー ユーザーズマニュアル',
				en: '2686V Manual',
			},
			// description は言語別に分けられないので、各ページ側で書き分ける
			description: 'レトロ音源風 VST「2686V」シリーズの使い方',
			// 日本語を既定にして、URL に言語を付けない。英語は /en/ の下。
			defaultLocale: 'root',
			locales: {
				root: { label: '日本語', lang: 'ja' },
				en: { label: 'English', lang: 'en' },
			},
			// サイドバーの開き具合。描き直す前に当てないと、閉じている人の画面で
			// 一瞬サイドバーが見えてしまう。
			head: [
				{
					tag: 'script',
					content: sidebarToggleScript,
				},
			],
			customCss: [
				'./src/styles/theme.css',
				'./src/styles/layout.css',
				'./src/styles/sidebar-toggle.css',
			],
			// ヘッダーの右端へ、サイドバーの表示切り替えを足す
			components: {
				SocialIcons: './src/components/SocialIcons.astro',
			},
			social: [
				{
					icon: 'github',
					label: 'GitHub',
					href: 'https://github.com/cyross/2686V_2686VFX',
				},
			],
			// 章立ては、書けたページから順に足していく。
			// 実在しないページを並べると、その場で失敗して気づける。
			sidebar: [
				{
					label: 'はじめに',
					translations: { en: 'Getting started' },
					items: [
						{ slug: 'guide/about' },
						{ slug: 'guide/features' },
						{ slug: 'guide/screen' },
						{ slug: 'guide/install' },
						{ slug: 'guide/changes' },
					],
				},
				{
					label: '各プラグインの紹介',
					translations: { en: 'Per plugin' },
					items: [
						{ slug: 'plugins/compare' },
						{ slug: 'plugins/2686v' },
						{ slug: 'plugins/2686vlight' },
						{ slug: 'plugins/26v' },
						{ slug: 'plugins/86v' },
						{ slug: 'plugins/opzx7s' },
						{ slug: 'plugins/fx-plugin' },
					],
				},
				{
					label: '基本仕様',
					translations: { en: 'Basic specifications' },
					items: [
						{ slug: 'reference/features' },
						{ slug: 'reference/signal-flow' },
						{ slug: 'reference/colors' },
					],
				},
				{
					label: '音源タブ',
					translations: { en: 'Sound chips' },
					// 章ごとに並べ直さずに済むよう、フォルダの中身をそのまま出す
					items: [{ autogenerate: { directory: 'chips' } }],
				},
				{
					label: 'FX',
					translations: { en: 'FX' },
					items: [{ autogenerate: { directory: 'fx' } }],
				},
				{
					label: '詳細仕様',
					translations: { en: 'Specification reference' },
					items: [
						{
							label: '各種一覧',
							translations: { en: 'List of features' },
							items: [
								{ slug: 'reference/lists-algorithm' },
								{ slug: 'reference/lists-waveform' },
								{ slug: 'reference/lists-quality' },
								{ slug: 'reference/color-names' },
								{ slug: 'reference/shortcuts' },
							],
						},
						{
							label: 'ファイル構成',
							translations: { en: 'File structure' },
							items: [
								{ slug: 'files/format' },
								{ slug: 'files/convert' },
							],
						},
						{
							label: 'ファイル仕様',
							translations: { en: 'File formats' },
							collapsed: true,
							items: [{ autogenerate: { directory: 'reference/file-spec' } }],
						},
						{
							label: 'オートメーション仕様',
							translations: { en: 'Automation formats' },
							collapsed: true,
							items: [{ autogenerate: { directory: 'reference/automation' } }],
						},
						{ slug: 'reference/mml' },
					],
				},
				{
					label: 'その他',
					translations: { en: 'Other specifications' },
					items: [
						{ slug: 'reference/license' },
						{ slug: 'reference/references' },
					],
				},
			],
		}),
	],
});
