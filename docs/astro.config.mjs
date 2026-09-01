// @ts-check
import { defineConfig } from 'astro/config';
import starlight from '@astrojs/starlight';

// GitHub Pages はリポジトリ名の下に置かれるので base が要る。
// https://cyross.github.io/2686V_2686VFX/
export default defineConfig({
	site: 'https://cyross.github.io',
	base: '/2686V_2686VFX',
	integrations: [
		starlight({
			title: {
				ja: '2686V マニュアル',
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
						{ slug: 'guide/install' },
						{ slug: 'guide/changes' },
					],
				},
				{
					label: '音源タブ',
					translations: { en: 'Sound chips' },
					// 章ごとに並べ直さずに済むよう、フォルダの中身をそのまま出す
					items: [{ autogenerate: { directory: 'chips' } }],
				},
				{
					label: 'ファイル',
					translations: { en: 'Files' },
					items: [
						{ slug: 'files/format' },
						{ slug: 'files/convert' },
					],
				},
				{
					label: '各種仕様',
					translations: { en: 'Reference' },
					items: [
						{ slug: 'reference/features' },
						{ slug: 'reference/signal-flow' },
						{ slug: 'reference/mml' },
						{ slug: 'reference/shortcuts' },
						{ slug: 'reference/colors' },
						{ slug: 'reference/license' },
						{ autogenerate: { directory: 'reference/automation' } },
					],
				},
				{
					label: 'プラグイン別',
					translations: { en: 'Per plugin' },
					items: [
						{ slug: 'plugins/compare' },
						{ slug: 'plugins/fx-plugin' },
					],
				},
			],
		}),
	],
});
