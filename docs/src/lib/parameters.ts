import data from '../data/parameters.json';

// パラメータ 1 件。ParamDump が書き出す形に合わせてある。
export interface Parameter {
	id: string;
	name: string;
	type: 'Int' | 'Float' | 'Bool' | 'Choice' | 'Other';
	min?: number | boolean;
	max?: number | boolean;
	step?: number;
	choices?: string[];
	default: number | boolean;
	unit?: string;
}

export const version: string = data.version;
export const total: number = data.count;

// どのチャンネルのものかは ID の先頭で分かる。
//
// 長いものから見ること。OPN で調べると OPNA まで拾ってしまう。
const GROUPS = [
	'OPNA', 'OPN', 'OPL3', 'OPL', 'OPM', 'OPZX7', 'SSG',
	'WTPLUS', 'WT2', 'WT', 'RHYTHM', 'ADPCM', 'BEEP', 'FX',
] as const;

export type Group = (typeof GROUPS)[number] | 'global';

export function groupOf(id: string): Group {
	for (const g of GROUPS) {
		if (id.startsWith(g + '_')) return g;
	}

	return 'global';
}

export function parametersOf(group: Group): Parameter[] {
	return (data.parameters as Parameter[]).filter((p) => groupOf(p.id) === group);
}

// 小数の刻みは 0.009999999776483 のような値になる。そのまま出すと読めない
// ので、意味のある桁だけ残す。
export function formatNumber(value: number): string {
	if (Number.isInteger(value)) return String(value);

	const rounded = Number(value.toPrecision(6));

	return String(rounded);
}

export function formatRange(p: Parameter, lang: 'ja' | 'en'): string {
	if (p.type === 'Bool') return lang === 'ja' ? 'False / True' : 'False / True';

	if (p.type === 'Choice') return (p.choices ?? []).join(' / ');

	if (typeof p.min !== 'number' || typeof p.max !== 'number') return '—';

	const range = `${formatNumber(p.min)} 〜 ${formatNumber(p.max)}`;

	return p.unit ? `${range} ${p.unit}` : range;
}

export function formatDefault(p: Parameter): string {
	if (typeof p.default === 'boolean') return p.default ? 'True' : 'False';

	if (p.type === 'Choice') {
		const index = Math.round(p.default);

		return p.choices?.[index] ?? String(index);
	}

	return formatNumber(p.default);
}
